#pragma once

#include "triangle.hpp"

#include <optional>
#include <vector>

class Mesh : public Shape {
public:
    Mesh() = default;
    Mesh(const std::vector<Triangle> &_triangles) : Mesh(_triangles, nullptr) {}
    Mesh(const std::vector<Triangle> &_triangles, Material *_material) : triangles(_triangles), material{_material} {}

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = 1e-5, float t_max = std::numeric_limits<float>::infinity()) const override;
    void addTriangle(const Triangle &tri) { triangles.push_back(tri); } // TODO: 得优化一下存取triangle的策略
    std::vector<Triangle> &getTriangles() { return triangles; }
    const std::vector<Triangle> &getTriangles() const { return triangles; }
    void setMaterial(Material *m) { material = m; }

private:
    std::vector<Triangle> triangles;
    Material *material{};

    std::optional<HitInfo> intersectBrutally(const Ray &ray, float t_min, float t_max) const;
};

