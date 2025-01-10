#include "shape/model.hpp"
#include <cassert>
#include <optional>
#include <rapidobj/rapidobj.hpp>

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

    return closest_hit;
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

    // 使用 rapidobj 库加载 obj 文件
    rapidobj::Result result = rapidobj::ParseFile(path, rapidobj::MaterialLibrary::Ignore());
    if (result.error) {
        throw std::runtime_error(std::format("Failed to load obj file: {}", result.error.line));
    }

    // 获取顶点、法线、纹理坐标
    const auto &positions = result.attributes.positions;
    const auto &normals = result.attributes.normals;

    // 遍历每个面，创建三角形
    for (const auto &shape : result.shapes) {
        // 创建一个 mesh 来存储三角形
        Mesh mesh;
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
            Triangle tri(p0, p1, p2, n0, n1, n2);
            mesh.addTriangle(tri);
        }
        // 将 mesh 添加到模型中
        meshes.emplace_back(mesh);
    }
}
void Model::loadObjOneMeshed(const std::filesystem::path &path) {
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

    // 创建一个 mesh 来存储三角形
    Mesh mesh;

    // 遍历每个面，创建三角形
    for (const auto &shape : result.shapes) {
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
            Triangle tri(p0, p1, p2, n0, n1, n2);
            mesh.addTriangle(tri);
        }
    }

    // 将 mesh 添加到模型中
    meshes.push_back(mesh);
}
