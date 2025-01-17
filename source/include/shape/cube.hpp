#pragma once

#include "shape/shape.hpp"
#include "shape/triangle.hpp"

class Cube: public Shape{
public:
    Cube(float edge, const Material *m);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = FLOAT_CMP_EPS, float t_max = std::numeric_limits<float>::infinity()) const override;
    std::optional<Bounds> getBounds() const override { return bounds; }

    void setMaterial(const Material *m) { material = m; }

private:
    std::vector<Triangle> triangles;
    Bounds bounds;
    const Material *material;
};