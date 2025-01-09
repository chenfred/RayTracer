#pragma once

#include "camera/ray.hpp"
#include <optional>

class Shape {
  public:
    virtual std::optional<HitInfo> intersect(const Ray &ray) const = 0;
};