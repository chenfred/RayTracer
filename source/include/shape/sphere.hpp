#pragma once

#include "camera/ray.hpp"
#include "shape.hpp"
#include <optional>
#include <stdexcept>

class Sphere : public Shape {
public:
    Sphere(float _r, const glm::vec3 &_c) : radius(_r), center(_c) {}

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = 1e-5, float t_max = std::numeric_limits<float>::infinity()) const override;
    void applyTransform(const glm::mat4 &transMat) override { throw std::runtime_error("Transform for sphere has not been implemented yet!"); }

private:
    float radius;
    glm::vec3 center;
};
