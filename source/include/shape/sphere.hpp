#pragma once

#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "shape.hpp"

#include <optional>

class Sphere : public Shape {
public:
    Sphere(float _r, const glm::vec3 &_c) : radius(_r), center(_c), bounds(_c - _r, _c + _r) {}

    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bounds getBounds() const override { return bounds; };

    void setMaterial(const Material *m) { material = m; }

private:
    float radius;
    glm::vec3 center;
    const Material *material;
    Bounds bounds;
};
