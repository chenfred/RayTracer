#pragma once

#include "shape.hpp"

class Plane : public Shape {
public:
    Plane(const glm::vec3 &_point, const glm::vec3 &_normal) : point(_point), normal(_normal) {}

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = FLOAT_CMP_EPS, float t_max = std::numeric_limits<float>::infinity()) const override;

    void setMaterial(const Material *m) { material = m; }

private:
    glm::vec3 point, normal;
    const Material *material;
};