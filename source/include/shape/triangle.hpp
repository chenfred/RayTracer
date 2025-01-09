#pragma once

#include "shape.hpp"

class Triangle : public Shape {
public:
    Triangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2,
             const glm::vec3 &n0, const glm::vec3 &n1, const glm::vec3 &n2);
    Triangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = 1e-5, float t_max = std::numeric_limits<float>::infinity()) const override;

private:
    glm::vec3 points[3], normals[3];
    glm::vec2 uvCoords[2];
};