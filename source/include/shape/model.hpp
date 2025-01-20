#pragma once

#include "shape/mesh.hpp"

#include <filesystem>
#include <vector>

class Model : public Shape {
public:
    Model(const std::filesystem::path &path, const Material *_m) : Model(loadObj(path, _m)) {}
    Model(const std::vector<Mesh> &meshes) : Model(std::vector<Mesh>(meshes)) {}
    Model(std::vector<Mesh> &&meshes);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = FLOAT_CMP_EPS, float t_max = std::numeric_limits<float>::infinity()) const override;
    std::optional<Bounds> getBounds() const override { return bvh.getBounds(); }
    void setCoveredMaterial(const Material *m) { coveredMaterial = m; }; // 为所有mesh设置相同的material

private:
    BVH<Mesh> bvh;
    const Material *coveredMaterial{};

    std::vector<Mesh> loadObj(const std::filesystem::path &path, const Material *m);
};
