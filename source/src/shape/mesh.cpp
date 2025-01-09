#include "shape/mesh.hpp"
#include "camera/ray.hpp"
#include <optional>

std::optional<HitInfo> Mesh::intersect(const Ray &ray, float t_min, float t_max) const {
    return intersectBrutally(ray, t_min, t_max);
}

// 暴力遍历求交法
// TODO: 之后会实现一个BVH求交
std::optional<HitInfo> Mesh::intersectBrutally(const Ray &ray, float t_min, float t_max) const {
    if (triangles.empty()) {
        return std::nullopt;
    }

    std::optional<HitInfo> closest_hit;
    float closest_t = t_max;
    for (const auto &triangle : triangles) {
        auto hit = triangle.intersect(ray, t_min, closest_t);
        if (hit) {
            closest_hit = hit;
            closest_t = hit->t;
        }
    }

    if (!closest_hit) {
        return std::nullopt;
    }
    return HitInfo{closest_hit->t, closest_hit->hitPoint, closest_hit->hitNormal};
}