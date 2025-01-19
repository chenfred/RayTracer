#include "shape/mesh.hpp"
#include "camera/ray.hpp"
#include "shape/triangle.hpp"
#include <optional>

Mesh::Mesh(const std::vector<Triangle> &_triangles, const Material *_material) : triangles(_triangles), material{_material} {
    for (const auto &tri : triangles) {
        bounds.expand(tri.getBounds().value());
    }
}

std::optional<HitInfo> Mesh::intersect(const Ray &ray, float t_min, float t_max) const {
    return intersectBrutally(ray, t_min, t_max);
}

// 暴力遍历求交法，之后会实现一个BVH求交
std::optional<HitInfo> Mesh::intersectBrutally(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closest_hit;
    float closet_t = t_max;
    for (const auto &triangle : triangles) {
        if(!triangle.getBounds()->hasIntersection(ray, t_min, closet_t)){
            continue;
        }

        auto hit = triangle.intersect(ray, t_min, closet_t);
        if (hit) {
            closest_hit = hit;
            closet_t = hit->t;
        }
    }

    if (!closest_hit) {
        return {};
    }
    return HitInfo{closest_hit->t, closest_hit->hitPoint, closest_hit->hitNormal, material};
}