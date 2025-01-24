#pragma once

#include "accelerate/bvh.hpp"
#include "shape/shape.hpp"
#include "shape/triangle.hpp"

class Cube: public Shape{
public:
    Cube(float edge, glm::vec3 center);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bounds getBounds() const override { return bvh.getBounds(); }

    void setMaterial(const Material *m) { material = m; }

private:
    BVH<Triangle> bvh;
    const Material *material{};
};