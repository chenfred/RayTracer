#include "accelerate/instance_bvh.hpp"
#include "shape/shape.hpp"

#include <algorithm>

std::optional<HitInfo> ShapeInstanceWrapper::intersect(const Ray &ray, float t_min, float t_max) const {
    if (instance.bounds.isValid() && !instance.bounds.hasIntersection(ray, t_min, t_max)) {
        return {};
    }

    // 为了保持相交time在model空间和在world空间的一致可比较，这里的transformedRay不能对dir进行归一化：ray在transMat的作用下有可能被拉长或压缩
    const auto ray_modelspace = ray.transformedRayAbnormalized(instance.world2modelMat);
    const auto hit_modelspace = instance.shape->intersect(ray_modelspace, t_min, t_max);
    if (!hit_modelspace) {
        return {};
    }

    auto hitPoint = transform_point(hit_modelspace->hitPoint, instance.model2worldMat);
    auto hitNormal = transform_normal(hit_modelspace->hitNormal, instance.world2modelMat);
    const Material *hitMaterial = instance.material ? instance.material : hit_modelspace->hitMaterial;

    HitInfo hit{hit_modelspace->t, hitPoint, hitNormal, hitMaterial, &instance};
    DEBUG_LINE(hit.boundsDepth = hit_modelspace->boundsDepth);
    DEBUG_LINE(hit.boundsTestCount = hit_modelspace->boundsTestCount);
    DEBUG_LINE(hit.shapeTestCount = hit_modelspace->shapeTestCount);
    return hit;
}

std::optional<HitInfo> InstanceBVH::intersect(const Ray &ray, float t_min, float t_max) const {
    glm::vec3 rayDirInv = 1.0f / ray.getDirection();
    std::optional<HitInfo> closestHit;
    DEBUG_LINE(uint16_t boundsTestCount{0}, shapeTestCount{0})

    // finite instances ====================================================================================

    // 用固定大小的栈来模拟递归调用
    std::array<uint32_t, BVH_INTERSECT_STACK_PREALLOC_SIZE> stack;
    uint8_t rsp{};    // 始终指向栈顶外侧一格（未使用的空间）
    stack[rsp++] = 0; // push
    while (rsp) {
        // 取栈顶参数
        auto currNodeIndex = stack[rsp - 1];
        const BVHNode &node = nodes[currNodeIndex];
        // 计算光线相对两个节点的方向(无论左右/上下/前后,都是位置较小的在lchild位置较大的在rchild)
        glm::ivec3 rayDirNeg = {ray.getDirection().x < 0, ray.getDirection().y < 0, ray.getDirection().z < 0};
        DEBUG_LINE(boundsTestCount++)
        if (!node.bounds.hasIntersection(ray, rayDirInv, t_min, t_max)) {
            rsp--; // 模拟退栈返回
        } else if (node.numEntities == 0) {
            rsp--; // 先退栈，再进接下来要执行的栈
            // 优化: 光线入射的那个方向的节点先相交测试,提前避开后面不必要的测试
            if (rayDirNeg[node.axis]) {
                // 先递归右子节点，再递归左子节点（与入栈顺序相反）
                stack[rsp++] = currNodeIndex + 1;
                stack[rsp++] = node.indexRChild;
            } else {
                // 先左后右
                stack[rsp++] = node.indexRChild;
                stack[rsp++] = currNodeIndex + 1;
            }
        } else {
            DEBUG_LINE(shapeTestCount += node.numEntities)
            int indexShapesBegin = node.indexEntitiesBegin; // 转为int防止int和uint比的时候出bug
            int indexShapesEnd = indexShapesBegin + node.numEntities;
            const auto f_intersectTestAndSet = [&](int i) -> void {
                const auto &wrapper = wrappers[i];
                auto hit = wrapper.intersect(ray, t_min, t_max);
                if (hit) {
                    t_max = hit->t;
                    closestHit = hit;
                    // DEBUG_PRINT(std::format("hit with bounds depth {}", node.nodeDepth))
                    DEBUG_LINE(closestHit->boundsDepth = std::max<decltype(closestHit->boundsDepth)>(closestHit->boundsDepth, node.depth))
                }
            };
            // 同样的优化: 光线入射的那个方向的节点先相交测试,提前避开后面不必要的测试
            if (rayDirNeg[node.axis]) {
                for (int i = indexShapesEnd - 1; i >= indexShapesBegin; --i) {
                    f_intersectTestAndSet(i);
                }
            } else {
                for (int i = node.indexEntitiesBegin; i < indexShapesEnd; ++i) {
                    f_intersectTestAndSet(i);
                }
            }
            rsp--; // 模拟退栈返回
        }
    }

    // infinity instances ====================================================================================

    for(const auto& wrapper:infInstWrappers){
        auto hit = wrapper.intersect(ray, t_min, t_max);
        if (hit) {
            t_max = hit->t;
            closestHit = hit;
            // DEBUG_PRINT(std::format("hit with bounds depth {}", node.nodeDepth))
        }
    }

    // return ================================================================================================

#ifdef WITH_DEBUG_INFO
    if (closestHit) {
        closestHit->boundsTestCount = std::max<decltype(closestHit->boundsTestCount)>(closestHit->boundsTestCount, boundsTestCount);
        closestHit->shapeTestCount = std::max<decltype(closestHit->shapeTestCount)>(closestHit->shapeTestCount, shapeTestCount);
    }
#endif
    return closestHit;
}

void InstanceBVH::build(std::vector<ShapeInstance> &&instances) {
    wrappers.reserve(instances.size());
    for (auto &instance : instances) {
        if (instance.bounds.isValid()) {
            wrappers.emplace_back(std::move(instance));
        } else {
            infInstWrappers.emplace_back(std::move(instance));
        }
    }
    wrappers.shrink_to_fit();
    nodes = BVHNodesBuilder<ShapeInstanceWrapper>::BuildBVHNodes(wrappers);
}