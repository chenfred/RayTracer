#include "shape/sphere.hpp"
#include "util/utils.hpp"

std::optional<HitInfo> Sphere::intersect(const Ray &ray, float t_min, float t_max) const {
    // 计算射线原点到球心的向量
    glm::vec3 oc = ray.getOrigin() - center;
    // 计算一元二次方程的系数
    float a = glm::dot(ray.getDirection(), ray.getDirection());
    float b = 2.0f * glm::dot(oc, ray.getDirection());
    float c = glm::dot(oc, oc) - radius * radius;
    // 计算判别式
    float discriminant = b * b - 4 * a * c;

    // 判别式小于0，无交点
    if (discriminant < 0) {
        return std::nullopt;
    }

    // 计算交点参数t
    float sqrtD = std::sqrt(discriminant);
    float t = (-b - sqrtD) / (2.0f * a);
    if (!in_range(t, t_min, t_max)) {
        t = (-b + sqrtD) / (2.0f * a);
        if (!in_range(t, t_min, t_max)) {
            return std::nullopt;
        }
    }

    // 计算交点信息
    glm::vec3 hitPoint = ray.getOrigin() + t * ray.getDirection();
    glm::vec3 hitNormal = glm::normalize(hitPoint - center);

    return HitInfo{t, hitPoint, hitNormal, material};
}
