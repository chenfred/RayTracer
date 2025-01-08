#pragma once

#include "camera/ray.hpp"
#include <limits>
#include <optional>

class Shape {
  public:
    virtual std::optional<HitInfo> intersect(const Ray &ray) const = 0;
};