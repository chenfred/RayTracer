#include "shape/plane.hpp"
#include "util/utils.hpp"
#include <optional>

std::optional<HitInfo> Plane::intersect(const Ray &ray, float t_min, float t_max) const {
    auto t = glm::dot(aPoint - ray.getOrigin(), normal) / glm::dot(ray.getDirection(), normal);

    if (!in_range(t, t_min, t_max)) {
        return std::nullopt;
    }
    return HitInfo{t, ray.hitAtPoint(t), normal, material};
}

// TODO: 这里也有法线变换
void Plane::applyTransform(const glm::mat4 &transMat) {
    glm::vec4 point_h = transMat * glm::vec4(aPoint, 1.0f);
    aPoint = glm::vec3(point_h) / point_h.w;

    glm::mat4 normalMat = glm::transpose(glm::inverse(transMat));
    normal = glm::normalize(normalMat * glm::vec4(normal, 0.0f));
}