#pragma once

#include "shape/mesh.hpp"

#include <filesystem>
#include <vector>

class Model : public Shape {
public:
    Model(const std::filesystem::path &path) : Model(loadObj(path)) {}
    Model(const std::vector<Mesh> &meshes) : Model(std::vector<Mesh>(meshes)) {}
    Model(std::vector<Mesh> &&meshes);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bounds getBounds() const override { return bvh.getBounds(); }
    
    void setFallbackMaterial(const Material *m) { fallbackMaterial = m; }; // 设置mesh无material时的默认值

private:
    BVH<Mesh> bvh;
    const Material *fallbackMaterial{};

    std::vector<Mesh> loadObj(const std::filesystem::path &path);
};
