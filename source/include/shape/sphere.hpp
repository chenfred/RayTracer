#pragma once

#include "camera/ray.hpp"
#include "shape.hpp"
#include <optional>

class Sphere : public Shape {
public:
    Sphere(float _r, const glm::vec3 &_c) : radius(_r), center(_c) {}

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = 1e-5, float t_max = std::numeric_limits<float>::infinity()) const override;

private:
    float radius;
    glm::vec3 center;
};