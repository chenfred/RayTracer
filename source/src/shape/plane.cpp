#include "shape/plane.hpp"
#include "util/utils.hpp"

#include <optional>

std::optional<HitInfo> Plane::intersect(const Ray &ray, float t_min, float t_max) const {
    auto t = glm::dot(point - ray.getOrigin(), normal) / glm::dot(ray.getDirection(), normal);

    if (!in_range(t, t_min, t_max)) {
        return std::nullopt;
    }
    return HitInfo{t, ray.hitAtPoint(t), normal, material};
}