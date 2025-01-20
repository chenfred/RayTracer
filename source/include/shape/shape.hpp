#pragma once

#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "util/globals.hpp"

#include <limits>
#include <optional>

class Shape {
public:
    Shape() = default;
    virtual ~Shape() = default;

    virtual std::optional<HitInfo> intersect(const Ray &ray, float t_min = FLOAT_CMP_EPS, float t_max = std::numeric_limits<float>::infinity()) const = 0;
    virtual std::optional<Bounds> getBounds() const { return std::nullopt; }
};

struct ShapeInstance {
    const Shape &shape;
    glm::mat4 model2worldMat, world2modelMat;
    std::optional<Bounds> bounds;
    const Material *material{};
    size_t index{INSTANCE_INDEX++};
};