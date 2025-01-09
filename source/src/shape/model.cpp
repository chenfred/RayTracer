#include "shape/model.hpp"
#include <cassert>
#include <optional>

Model::Model(const std::vector<Triangle> &_triangles, Material *_material) {
    Mesh mesh{_triangles, _material};
    meshes.reserve(1);
    meshes.push_back(mesh);
}

std::optional<HitInfo> Model::intersect(const Ray &ray, float t_min, float t_max) const {
    if (meshes.empty()) {
        return std::nullopt;
    }

    std::optional<HitInfo> closest_hit;
    float closest_t = t_max;
    for (const auto &mesh : meshes) {
        auto hit = mesh.intersect(ray, t_min, closest_t);
        if (hit) {
            closest_hit = hit;
            closest_t = hit->t;
        }
    }

    if (!closest_hit) {
        return std::nullopt;
    }
    return HitInfo{closest_hit->t, closest_hit->hitPoint, closest_hit->hitNormal};
}

void Model::addTriangle(const Triangle &tri) {
    assert(meshes.size() == 1);
    meshes[0].addTriangle(tri);
}

std::vector<Triangle> &Model::getTriangles() {
    assert(meshes.size() == 1);
    return meshes[0].getTriangles();
}
const std::vector<Triangle> &Model::getTriangles() const {
    assert(meshes.size() == 1);
    return meshes[0].getTriangles();
}

void Model::setMaterial(Material *m) {
    assert(meshes.size() == 1);
    meshes[0].setMaterial(m);
}

void Model::loadObj(const std::filesystem::path &path) {
    std::string ext = path.extension().string();
    if (ext != ".obj") {
        throw std::runtime_error(std::format("Object format {} not implemented yet.", ext));
    }

    // TODO: 找个库加载obj文件，先不考虑材质吧
}