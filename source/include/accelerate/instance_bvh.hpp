#pragma once

#include "shape/shape.hpp"
#include "accelerate/bvh_builder.hpp"

class ShapeInstanceWrapper : public Shape {
public:
    ShapeInstanceWrapper(ShapeInstance &&_instance) : instance{std::move(_instance)} {}

    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bounds getBounds() const override { return instance.bounds; }

private:
    ShapeInstance instance;
};

class InstanceBVH : public Shape {
public:
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;

    void build(std::vector<ShapeInstance> &&instances);

private:
    std::vector<ShapeInstanceWrapper> wrappers;
    std::vector<BVHNode> nodes;
    std::vector<ShapeInstanceWrapper> infInstWrappers;
};