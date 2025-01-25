#pragma once

#include "accelerate/bvh_builder.hpp"
#include "camera/ray.hpp"
#include "shape/shape.hpp"

#include <cassert>
#include <cstdint>
#include <format>
#include <optional>
#include <vector>

template <ShapeType T>
class BVH : public Shape {
public:
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bounds getBounds() const override;

    void build(std::vector<T> &&_shapes) {
        shapes = std::move(_shapes);
        nodes = BVHBuilder<T>::BuildBVHNodes(shapes);
    }

private:
    std::vector<T> shapes;
    std::vector<BVHNode> nodes;
};

template <ShapeType T>
std::optional<HitInfo> BVH<T>::intersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closestHit;
    DEBUG_LINE(uint16_t boundsTestCount{0}, shapeTestCount{0})
    // 用栈来模拟递归调用
    std::vector<uint32_t> stack;
    stack.reserve(BVH_INTERSECT_STACK_PREALLOC_SIZE);
    stack.push_back(0);
    while (!stack.empty()) {
        // 取栈顶参数
        auto currNodeIndex = stack.back();
        const BVHNode &node = nodes[currNodeIndex];
        DEBUG_LINE(boundsTestCount++)
        if (!node.bounds.hasIntersection(ray, t_min, t_max)) {
            // 退栈模拟返回
            stack.pop_back();
        } else if (node.numShapes == 0) {
            // (递归左子节点，递归右子节点，返回）逆着来
            stack.pop_back();
            stack.push_back(node.indexRChild);
            stack.push_back(currNodeIndex + 1);
        } else {
            DEBUG_LINE(shapeTestCount += node.numShapes)
            auto indexShapesEnd = node.indexShapesBegin + node.numShapes;
            for (auto i = node.indexShapesBegin; i < indexShapesEnd; ++i) {
                const Shape &shape = shapes[i];
                std::optional<HitInfo> hit = shape.intersect(ray, t_min, t_max);
                if (hit) {
                    t_max = hit->t;
                    closestHit = hit;
                    // DEBUG_PRINT(std::format("hit with bounds depth {}", node.nodeDepth))
                    DEBUG_LINE(closestHit->boundsDepth = std::max(closestHit->boundsDepth, node.depth))
                }
            }
            stack.pop_back(); // 返回
        }
    }

    DEBUG_PRINT(std::format("BVH::Intersect: Stack expand to {}", stack.capacity()), stack.capacity() > BVH_INTERSECT_STACK_PREALLOC_SIZE)
#ifdef WITH_DEBUG_INFO
    if (closestHit) {
        closestHit->boundsTestCount = std::max(closestHit->boundsTestCount, boundsTestCount);
        closestHit->shapeTestCount = std::max(closestHit->shapeTestCount, shapeTestCount);
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