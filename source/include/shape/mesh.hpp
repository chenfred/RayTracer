#pragma once

#include "triangle.hpp"
#include "accelerate/bvh.hpp"

#include <optional>
#include <vector>

class Mesh : public Shape {
public:
    Mesh(std::vector<Triangle> &&_tri, const Material *_m);
    Mesh(const std::vector<Triangle> &_tri, const Material *_m);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = FLOAT_CMP_EPS, float t_max = std::numeric_limits<float>::infinity()) const override;
    std::optional<Bounds> getBounds() const override {return bvh.getBounds();}
    
    void setMaterial(const Material *m) { material = m; }

private:
    std::vector<Triangle> triangles;
    const Material *material;
    Bounds bounds;
    BVH<Triangle> bvh;

    std::optional<HitInfo> intersectWithBVH(const Ray &ray, float t_min, float t_max) const;
    std::optional<HitInfo> intersectBrutally(const Ray &ray, float t_min, float t_max) const;
};
