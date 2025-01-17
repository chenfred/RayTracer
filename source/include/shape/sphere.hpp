#pragma once

#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "shape.hpp"

#include <optional>

class Sphere : public Shape {
public:
    Sphere(float _r, const glm::vec3 &_c, const Material *_m) : radius(_r), center(_c), bounds(_c - _r, _c + _r), material(_m) {}

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = 0, float t_max = std::numeric_limits<float>::infinity()) const override;
    std::optional<Bounds> getBounds() const override { return {}; };

    void setMaterial(const Material *m) { material = m; }

private:
    float radius;
    glm::vec3 center;
    const Material *material;
    Bounds bounds;
};
