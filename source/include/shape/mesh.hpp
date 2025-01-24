#pragma once

#include "accelerate/bvh.hpp"
#include "triangle.hpp"

#include <optional>
#include <vector>

class Mesh : public Shape {
public:
    Mesh(const std::vector<Triangle> &triangles) : Mesh(std::vector<Triangle>(triangles)) {}
    Mesh(std::vector<Triangle> &&triangles);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bounds getBounds() const override { return bvh.getBounds(); }

    void setMaterial(const Material *m) { material = m; }

private:
    BVH<Triangle> bvh;
    const Material *material;
};
