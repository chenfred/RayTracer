#pragma once

#include "accelerate/bounds.hpp"
#include "shape.hpp"

#include <array>

class Triangle : public Shape {
public:
    Triangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2,
             const glm::vec3 &n0, const glm::vec3 &n1, const glm::vec3 &n2);
    Triangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bounds getBounds() const override { return bounds; }

private:
    std::array<glm::vec3, 3> points, normals;
    std::array<glm::vec2, 3> uvCoords;
    Bounds bounds;

    Bounds buildBounds();
};
