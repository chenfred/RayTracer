#include "shape/mesh.hpp"
#include "camera/ray.hpp"
#include "shape/triangle.hpp"
#include <optional>

// TEST: 测试BVH的实现
Mesh::Mesh(std::vector<Triangle> &&_tri, const Material *_m) : material{_m} {
    bvh.build(std::move(_tri));
}

Mesh::Mesh(const std::vector<Triangle> &_tri, const Material *_m) : triangles{_tri}, material{_m} {
    bvh.build(std::vector<Triangle>(triangles));
}

std::optional<HitInfo> Mesh::intersect(const Ray &ray, float t_min, float t_max) const {
    return intersectWithBVH(ray, t_min, t_max);
    // return intersectBrutally(ray, t_min, t_max);
}

std::optional<HitInfo> Mesh::intersectWithBVH(const Ray &ray, float t_min, float t_max) const {
    auto hit = bvh.intersect(ray, t_min, t_max);
    if (!hit) {
        return {};
    }
    hit->hitMaterial = material;
    return hit;
}

std::optional<HitInfo> Mesh::intersectBrutally(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closest_hit;
    float closet_t = t_max;
    for (const auto &triangle : triangles) {
        if (!triangle.getBounds()->hasIntersection(ray, t_min, closet_t)) {
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