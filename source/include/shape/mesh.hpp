#pragma once

#include "triangle.hpp"

#include <optional>
#include <vector>

class Mesh : public Shape {
public:
    Mesh(const std::vector<Triangle> &_triangles, const Material *_material);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = 1e-5, float t_max = std::numeric_limits<float>::infinity()) const override;
    std::optional<Bounds> getBounds() const override {return bounds;}
    
    void setMaterial(const Material *m) { material = m; }

private:
    std::vector<Triangle> triangles;
    const Material *material;
    Bounds bounds;

    std::optional<HitInfo> intersectBrutally(const Ray &ray, float t_min, float t_max) const;
};
