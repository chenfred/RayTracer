#pragma once

#include "shape/shape.hpp"

#include <array>

struct BVHNode {
    Bounds bounds;
    std::vector<ShapeInstance> instances;
    std::array<BVHNode *, 2> children;

    void updateBounds() {
        bounds = {}; // 先清空
        for (const auto &instance : instances) {
            if (instance.bounds) {
                bounds.expand(instance.bounds.value());
            }
        }
    }
};