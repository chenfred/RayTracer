#pragma once

#include "accelerate/bvh_builder.hpp"
#include "camera/ray.hpp"
#include "func/debug_helpers.hpp"
#include "func/tools.hpp"
#include "shape/shape.hpp"

#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>

template <ShapeType T>
class BVH : public Shape {
public:
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bounds getBounds() const override;

    void build(std::vector<T> &&_shapes) {
        shapes = std::move(_shapes);
        nodes = BVHNodesBuilder<T>::BuildBVHNodes(shapes);
    }

private:
    std::vector<T> shapes;
    std::vector<BVHNode> nodes;
};

template <ShapeType T>
std::optional<HitInfo> BVH<T>::intersect(const Ray &ray, float t_min, float t_max) const {
    glm::vec3 rayDirInv = 1.0f / ray.getDirection();
    std::optional<HitInfo> closestHit;
    DEBUG_LINE(uint16_t boundsTestCount{0}, shapeTestCount{0})
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
        } else if (node.numShapes == 0) {
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
            DEBUG_LINE(shapeTestCount += node.numShapes)
            int indexShapesBegin = node.indexShapesBegin; // 转为int防止int和uint比的时候出bug
            int indexShapesEnd = indexShapesBegin + node.numShapes;
            const auto f_intersectTest = [&](int i) -> void {
                const Shape &shape = shapes[i];
                std::optional<HitInfo> hit = shape.intersect(ray, t_min, t_max);
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
                    f_intersectTest(i);
                }
            } else {
                for (int i = node.indexShapesBegin; i < indexShapesEnd; ++i) {
                    f_intersectTest(i);
                }
            }
            rsp--; // 模拟退栈返回
        }
    }

#ifdef WITH_DEBUG_INFO
    if (closestHit) {
        closestHit->boundsTestCount = std::max<decltype(closestHit->boundsTestCount)>(closestHit->boundsTestCount, boundsTestCount);
        closestHit->shapeTestCount = std::max<decltype(closestHit->shapeTestCount)>(closestHit->shapeTestCount, shapeTestCount);
    }
#endif
    return closestHit;
}

template <ShapeType T>
Bounds BVH<T>::getBounds() const {
    if (nodes.empty()) {
        return {};
    }
    return nodes.front().bounds;
}