#pragma once

#include "shape.hpp"

#include <vector>

// TODO: 实现BVH管理所有ShapeInstance
class Scene : public Shape {
public:
    // 采用扭曲ray.dir（不进行归一化）来解决在modelspace和worldspace的光线行进时间不一致问题
    std::optional<HitInfo> intersect(const Ray &ray, float t_min = FLOAT_CMP_EPS, float t_max = std::numeric_limits<float>::infinity()) const override;
    void addShape(const Shape &shape,
                  const glm::vec3 &pos = {0, 0, 0},
                  const glm::vec3 &scale = {1, 1, 1},
                  const glm::vec3 &rotate = {0, 0, 0}) { addShape(shape, pos, scale, rotate, nullptr); }
    void addShape(const Shape &shape, const glm::vec3 &pos, const glm::vec3 &scale, const glm::vec3 &rotate, const Material *material);
    void addShape(const Shape &shape, const glm::mat4 model2worldMat, const Material *material);
    void addShapeInstance(const ShapeInstance &instance) { instances.push_back(instance); }

private:
    std::vector<ShapeInstance> instances;

    // 直接转换time来处理worldspace和modelspace的光线时间不一致问题（备用）
    std::optional<HitInfo> intersectTransformTime(const Ray &ray, float t_min, float t_max) const;
};