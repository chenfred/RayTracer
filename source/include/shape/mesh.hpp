#pragma once

#include "accelerate/bvh.hpp"
#include "triangle.hpp"

#include <optional>
#include <vector>

class Mesh : public Shape {
public:
    Mesh(const std::vector<Triangle> &triangles, const Material *_m) : Mesh(std::vector<Triangle>(triangles), _m) {}
    Mesh(std::vector<Triangle> &&triangles, const Material *_m);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = FLOAT_CMP_EPS, float t_max = std::numeric_limits<float>::infinity()) const override;
    std::optional<Bounds> getBounds() const override { return bvh.getBounds(); }

    void setMaterial(const Material *m) { material = m; }

private:
    BVH<Triangle> bvh;
    const Material *material;
};
