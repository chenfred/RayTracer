#pragma once

#include "shape.hpp"

#include <vector>

// TODO: 理一下addShape的逻辑
class Scene : public Shape {
public:
    std::optional<HitInfo> intersect(const Ray &ray, float t_min = 0, float t_max = std::numeric_limits<float>::infinity()) const override;

    void addShape(const Shape &shape,
                  const glm::vec3 &pos = {0, 0, 0},
                  const glm::vec3 &scale = {1, 1, 1},
                  const glm::vec3 &rotate = {0, 0, 0}) { addShape(shape, pos, scale, rotate, nullptr); }
    void addShape(const Shape &shape, const glm::vec3 &pos, const glm::vec3 &scale, const glm::vec3 &rotate, const Material *material);
    void addShape(const Shape &shape, const glm::mat4 transMat, const Material *material);
    void addShapeInstance(const ShapeInstance &instance) { instances.push_back(instance); }

private:
    std::vector<ShapeInstance> instances;
};