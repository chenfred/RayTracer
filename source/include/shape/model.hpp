#pragma once

#include "accelerate/bounds.hpp"
#include "shape/mesh.hpp"
#include "shape/shape.hpp"
#include "triangle.hpp"

#include <cassert>
#include <filesystem>
#include <vector>

class Model : public Shape {
public:
    Model() = default;
    Model(const std::filesystem::path &path) { loadObj(path); }
    Model(const std::vector<Mesh> &_meshes);
    Model(const std::vector<Triangle> &_triangles) : Model{_triangles, nullptr} {}
    Model(const std::vector<Triangle> &_triangles, Material *_material); // 简单Model模式，只存一个mesh

    std::optional<HitInfo> intersect(const Ray &ray, float t_min = 1e-5, float t_max = std::numeric_limits<float>::infinity()) const override;
    void applyTransform(const glm::mat4 &transMat) override;
    void addMesh(const Mesh &mesh);
    std::vector<Mesh> &getMeshes() { return meshes; }
    const std::vector<Mesh> &getMeshes() const { return meshes; }
    Bounds getBounds() const override { return bounds; }
    void setMaterial(Material *m);

    // 简单Model模式
    void addTriangle(const Triangle &tri);
    std::vector<Triangle> &getTriangles();
    const std::vector<Triangle> &getTriangles() const;

private:
    std::vector<Mesh> meshes;
    Bounds bounds{};

    void loadObj(const std::filesystem::path &path);
    void loadObjOneMeshed(const std::filesystem::path &path);
};
