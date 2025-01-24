#include "shape/cube.hpp"
#include "shape/triangle.hpp"
#include "util/timer.hpp"

#include <cassert>

// 定义正方体的6个面，每个面由2个三角形组成
// 每个三角形的顶点按逆时针顺序排列，以确保法线朝外
static constexpr std::array<std::array<int, 3>, 12> faceTriangles = {{
    {{0, 3, 2}},
    {{0, 2, 1}}, // -Z face
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

Cube::Cube(float edge, glm::vec3 center) {
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

    // 构建三角形容器
    std::vector<Triangle> triangles;
    triangles.reserve(12);
    for (const auto &face : faceTriangles) {
        triangles.emplace_back(vertices[face[0]], vertices[face[1]], vertices[face[2]]);
    }

    // 构建bvh
    Timer buildTimer{"Build cube BVH"};
    bvh.build(std::move(triangles));
    buildTimer.conclude();
}

std::optional<HitInfo> Cube::intersect(const Ray &ray, float t_min, float t_max) const {
    auto hit = bvh.intersect(ray, t_min, t_max);
    if (!hit) {
        return {};
    }
    hit->hitMaterial = material;
    return hit;
}