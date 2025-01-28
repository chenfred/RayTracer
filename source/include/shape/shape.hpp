#pragma once

#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "util/globals.hpp"

#include <optional>

class Shape {
public:
    Shape() = default;
    virtual ~Shape() = default;

    virtual std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const = 0;
    virtual Bounds getBounds() const { return {}; }
};

// TODO: 管理好指针和引用
struct ShapeInstance {
    const Shape *shape;
    glm::mat4 model2worldMat, world2modelMat;
    Bounds bounds{};
    const Material *material{};
    size_t index{INSTANCE_INDEX++};
};