#pragma once

#include "accelerate/bounds.hpp"
#include "shape/mesh.hpp"
#include "shape/shape.hpp"

#include <filesystem>
#include <vector>

class Model : public Shape {
public:
    Model(const std::filesystem::path &path, const Material* m) : Model{loadObj(path, m)} {}
    Model(const std::vector<Mesh> &_meshes);

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = FLOAT_CMP_EPS, float t_max = std::numeric_limits<float>::infinity()) const override;
    std::optional<Bounds> getBounds() const override { return bounds; }
    void setMaterial(const Material *m); // 为所有mesh设置相同的material

private:
    std::vector<Mesh> meshes;
    Bounds bounds{};

    std::vector<Mesh> loadObj(const std::filesystem::path &path, const Material* m);
};
