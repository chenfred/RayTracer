#pragma once

#include "shape.hpp"

class Plane : public Shape {
public:
    Plane(const glm::vec3 &_aPoint, const glm::vec3 &_normal) : aPoint(_aPoint), normal(_normal), material(nullptr) {}
    Plane(const glm::vec3 &_aPoint, const glm::vec3 &_normal, Material *_material) : aPoint(_aPoint), normal(_normal), material(_material) {}

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = 1e-5, float t_max = std::numeric_limits<float>::infinity()) const override;
    void applyTransform(const glm::mat4 &transMat) override;
    
private:
    glm::vec3 aPoint, normal;
    Material *material{};
};