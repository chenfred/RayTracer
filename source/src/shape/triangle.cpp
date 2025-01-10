#include "shape/triangle.hpp"
#include "util/utils.hpp"

#include <glm/geometric.hpp>

Triangle::Triangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2,
                   const glm::vec3 &n0, const glm::vec3 &n1, const glm::vec3 &n2) {
    points[0] = p0;
    points[1] = p1;
    points[2] = p2;
    normals[0] = n0;
    normals[1] = n1;
    normals[2] = n2;
}

Triangle::Triangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2) {
    points[0] = p0;
    points[1] = p1;
    points[2] = p2;
    auto e1 = p1 - p0;
    auto e2 = p2 - p0;
    auto n = glm::normalize(glm::cross(e1, e2));
    for (auto &ni : normals) {
        ni = n;
    }
}

std::optional<HitInfo> Triangle::intersect(const Ray &ray, float t_min, float t_max) const {
    // Möller–Trumbore intersection algorithm
    const glm::vec3 &v0 = points[0];
    const glm::vec3 &v1 = points[1];
    const glm::vec3 &v2 = points[2];

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(ray.getDirection(), edge2);
    float a = glm::dot(edge1, h);

    // If the determinant is near zero, the ray lies in the plane of the triangle
    if (a > -1e-5 && a < 1e-5) {
        return std::nullopt;
    }

    float f = 1.0f / a;
    glm::vec3 s = ray.getOrigin() - v0;
    float u = f * glm::dot(s, h);

    if (u < 0.0f || u > 1.0f) {
        return std::nullopt;
    }

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(ray.getDirection(), q);

    if (v < 0.0f || u + v > 1.0f) {
        return std::nullopt;
    }

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * glm::dot(edge2, q);

    if (!in_range(t, t_min, t_max)) {
        return std::nullopt;
    }

    // Compute the barycentric coordinates
    float w = 1.0f - u - v;

    // Calculate the intersection point
    glm::vec3 hitPoint = w * v0 + u * v1 + v * v2;

    // Calculate the normal at the intersection point
    glm::vec3 hitNormal = glm::normalize(w * normals[0] + u * normals[1] + v * normals[2]);

    return HitInfo{t, hitPoint, hitNormal};
}

