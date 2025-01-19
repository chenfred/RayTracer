#include "shape/cube.hpp"
#include "shape/triangle.hpp"

#include <cassert>

Cube::Cube(float edge, glm::vec3 center, const Material *m) : material{m} {
    triangles.reserve(12);

    // 计算正方体顶点坐标
    float halfEdge = edge / 2.0f;
    std::array<glm::vec3, 8> vertices{
        glm::vec3(-halfEdge, -halfEdge, -halfEdge) + center,
        glm::vec3(halfEdge, -halfEdge, -halfEdge) + center,
        glm::vec3(halfEdge, halfEdge, -halfEdge) + center,
        glm::vec3(-halfEdge, halfEdge, -halfEdge) + center,
        glm::vec3(-halfEdge, -halfEdge, halfEdge) + center,
        glm::vec3(halfEdge, -halfEdge, halfEdge) + center,
        glm::vec3(halfEdge, halfEdge, halfEdge) + center,
        glm::vec3(-halfEdge, halfEdge, halfEdge) + center};

    // 定义正方体的6个面，每个面由2个三角形组成
    // 每个三角形的顶点按逆时针顺序排列，以确保法线朝外
    std::array<std::array<int, 3>, 12> faceTriangles = {{
        {{0, 3, 2}}, {{0, 2, 1}}, // -Z face
        {{4, 5, 6}},
        {{4, 6, 7}}, // +Z face
        {{0, 1, 5}},
        {{0, 5, 4}}, // -Y face
        {{2, 3, 7}},
        {{2, 7, 6}}, // +Y face
        {{0, 7, 3}},
        {{0, 4, 7}}, // -X face
        {{1, 2, 6}},
        {{1, 6, 5}} // +X face
    }};

    // 添加三角形到triangles向量中
    for (const auto &face : faceTriangles) {
        triangles.emplace_back(vertices[face[0]], vertices[face[1]], vertices[face[2]]);
    }

    bounds = Bounds{vertices[0], vertices[6]};
}

std::optional<HitInfo> Cube::intersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closet_hit;
    float closet_t = t_max;
    for (const auto &triangle : triangles) {
        if (!triangle.getBounds()->hasIntersection(ray, t_min, closet_t)) {
            continue;
        }

        auto hitInfo = triangle.intersect(ray, t_min, closet_t);
        if (hitInfo) {
            closet_hit = hitInfo;
            closet_t = hitInfo->t;
        }
    }

    if (!closet_hit) {
        return {};
    }
    return HitInfo{closet_t, closet_hit->hitPoint, closet_hit->hitNormal, material};
}