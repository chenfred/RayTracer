#pragma once

#include "camera/ray.hpp"
#include <limits>
#include <optional>

class Shape {
public:
    Shape() = default;
    virtual ~Shape() = default;

    virtual std::optional<HitInfo> intersect(const Ray &ray, float t_min = 1e-5, float t_max = std::numeric_limits<float>::infinity()) const = 0;
};