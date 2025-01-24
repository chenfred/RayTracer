#pragma once

#include "shape.hpp"

#include <vector>

// TODO: 实现BVH管理所有ShapeInstance
class Scene : public Shape {
public:
    // 采用扭曲ray.dir（不进行归一化）来解决在modelspace和worldspace的光线行进时间不一致问题
    std::optional<HitInfo> intersect(const Ray &ray, float t_min = FLOAT_LOOSE_POS_ZERO, float t_max = std::numeric_limits<float>::infinity()) const override;

    void addShapeInstance(const ShapeInstance &instance) { instances.push_back(instance); }
    void addShape(const Shape &shape, const Material *material,
                  const glm::vec3 &pos = {0, 0, 0},
                  const glm::vec3 &scale = {1, 1, 1},
                  const glm::vec3 &rotate = {0, 0, 0});
    void addShape(const Shape &shape, const Material *material, const glm::mat4 model2worldMat);

private:
    std::vector<ShapeInstance> instances;

    // 直接转换time来处理worldspace和modelspace的光线时间不一致问题（备用）
    std::optional<HitInfo> intersectTransformTime(const Ray &ray, float t_min, float t_max) const;
};