#include "shape/model.hpp"
#include "shape/triangle.hpp"
#include "util/timer.hpp"

#include <cassert>
#include <optional>
#include <rapidobj/rapidobj.hpp>

Model::Model(std::vector<Mesh> &&meshes) {
    Timer timer{"build model bvh"};
    bvh.build(std::move(meshes));
    timer.conclude();
}

std::optional<HitInfo> Model::intersect(const Ray &ray, float t_min, float t_max) const {
    auto hit = bvh.intersect(ray, t_min, t_max);
    if (!hit) {
        return {};
    }
    if (coveredMaterial) {
        assert(false);
        hit->hitMaterial = coveredMaterial;
    }
    return hit;

    // std::optional<HitInfo> closest_hit;
    // float closet_t = t_max;
    // for (const auto &mesh : meshes) {
    //     if (!mesh.getBounds()->hasIntersection(ray, t_min, closet_t)) {
    //         continue;
    //     }
    //     auto hit = mesh.intersect(ray, t_min, closet_t);
    //     if (hit) {
    //         closest_hit = hit;
    //         closet_t = hit->t;
    //     }
    // }

    // return closest_hit;
}

std::vector<Mesh> Model::loadObj(const std::filesystem::path &path, const Material *m) {
    std::string ext = path.extension().string();
    if (ext != ".obj") {
        throw std::runtime_error(std::format("Object format {} not implemented yet.", ext));
    }

    // 使用 rapidobj 库加载 obj 文件
    rapidobj::Result result = rapidobj::ParseFile(path, rapidobj::MaterialLibrary::Ignore());
    if (result.error) {
        throw std::runtime_error(std::format("Failed to load obj file: {}", result.error.line));
    }

    // 获取顶点、法线、纹理坐标
    const auto &positions = result.attributes.positions;
    const auto &normals = result.attributes.normals;

    // 创建返回值
    std::vector<Mesh> meshes;
    // 缓存三角形的容器
    std::vector<Triangle> triangles;
    // 遍历每个面，创建三角形
    for (const auto &shape : result.shapes) {
        std::vector<Triangle> triangles;
        for (size_t i = 0; i < shape.mesh.num_face_vertices.size(); ++i) {
            if (shape.mesh.num_face_vertices[i] != 3) {
                throw std::runtime_error("Only triangular faces are supported.");
            }

            // 获取面的三个顶点索引
            size_t index1 = shape.mesh.indices[i * 3 + 0].position_index;
            size_t index2 = shape.mesh.indices[i * 3 + 1].position_index;
            size_t index3 = shape.mesh.indices[i * 3 + 2].position_index;

            // 获取顶点坐标
            glm::vec3 p0(positions[index1 * 3 + 0], positions[index1 * 3 + 1], positions[index1 * 3 + 2]);
            glm::vec3 p1(positions[index2 * 3 + 0], positions[index2 * 3 + 1], positions[index2 * 3 + 2]);
            glm::vec3 p2(positions[index3 * 3 + 0], positions[index3 * 3 + 1], positions[index3 * 3 + 2]);

            // 获取法线
            glm::vec3 n0, n1, n2;
            if (shape.mesh.indices[i * 3 + 0].normal_index >= 0) {
                n0 = glm::vec3(normals[shape.mesh.indices[i * 3 + 0].normal_index * 3 + 0],
                               normals[shape.mesh.indices[i * 3 + 0].normal_index * 3 + 1],
                               normals[shape.mesh.indices[i * 3 + 0].normal_index * 3 + 2]);
            } else {
                n0 = glm::vec3(0.0f);
            }
            if (shape.mesh.indices[i * 3 + 1].normal_index >= 0) {
                n1 = glm::vec3(normals[shape.mesh.indices[i * 3 + 1].normal_index * 3 + 0],
                               normals[shape.mesh.indices[i * 3 + 1].normal_index * 3 + 1],
                               normals[shape.mesh.indices[i * 3 + 1].normal_index * 3 + 2]);
            } else {
                n1 = glm::vec3(0.0f);
            }
            if (shape.mesh.indices[i * 3 + 2].normal_index >= 0) {
                n2 = glm::vec3(normals[shape.mesh.indices[i * 3 + 2].normal_index * 3 + 0],
                               normals[shape.mesh.indices[i * 3 + 2].normal_index * 3 + 1],
                               normals[shape.mesh.indices[i * 3 + 2].normal_index * 3 + 2]);
            } else {
                n2 = glm::vec3(0.0f);
            }

            // 创建三角形并添加到 mesh 中
            triangles.emplace_back(p0, p1, p2, n0, n1, n2);
        }
        // 将 mesh 添加到模型中
        meshes.emplace_back(std::move(triangles), m);
    }

    return meshes;
}