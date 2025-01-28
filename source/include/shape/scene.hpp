#pragma once

#include "accelerate/instance_bvh.hpp"
#include "shape.hpp"

#include <vector>

class Scene : public Shape {
public:
    bool isBuilt() const { return built; }
    // 采用扭曲ray.dir（不进行归一化）来解决在modelspace和worldspace的光线行进时间不一致问题
    std::optional<HitInfo> intersect(const Ray &ray, float t_min = FLOAT_LOOSE_POS_ZERO, float t_max = std::numeric_limits<float>::infinity()) const override;

    void addShape(const Shape &shape, const Material *material,
                  const glm::vec3 &pos = {0, 0, 0},
                  const glm::vec3 &scale = {1, 1, 1},
                  const glm::vec3 &rotate = {0, 0, 0});
    void addShape(const Shape &shape, const Material *material, const glm::mat4 model2worldMat);

    void build();

private:
    bool built{false};
    InstanceBVH instanceBVH;
    std::vector<ShapeInstance> instances;

    void moveInstance(ShapeInstance &&instance) { instances.emplace_back(instance); }
};