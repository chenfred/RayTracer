#include "shape/mesh.hpp"
#include "camera/ray.hpp"
#include "shape/triangle.hpp"
#include "util/timer.hpp"

#include <optional>

Mesh::Mesh(std::vector<Triangle> &&triangles) {
    Timer timer{"build mesh BVH"};
    bvh.build(std::move(triangles));
    timer.conclude();
}

std::optional<HitInfo> Mesh::intersect(const Ray &ray, float t_min, float t_max) const {
    auto hit = bvh.intersect(ray, t_min, t_max);
    if (!hit) {
        return {};
    }
    hit->hitMaterial = material;
    return hit;
}