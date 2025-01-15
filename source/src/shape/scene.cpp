#include "shape/scene.hpp"
#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "glm/matrix.hpp"
#include "shape/shape.hpp"
#include "util/debug.hpp"

#include <cassert>
#include <glm/ext/matrix_transform.hpp>
#include <optional>

std::optional<HitInfo> Scene::intersect(const Ray &ray, float t_min, float t_max) const {
    if (instances.empty()) {
        return std::nullopt;
    }

    std::optional<HitInfo> closet_hit_modelspace;
    const ShapeInstance *closet_instance;
    float closet_t = t_max;
    for (const auto &instance : instances) {
        const auto bounds = instance.bounds;
        if (bounds && !bounds->hasIntersection(ray, t_min, closet_t)) {
            continue;
        }

        const auto ray_modelspace = ray.transformedRay(instance.world2modelMat);

        // if (ray != ray_modelspace) {
        //     ray.print();
        //     ray_modelspace.print();
        //     std::cout << "======================" << std::endl;
        // }

        const auto hit_modelspace = instance.shape.intersect(ray_modelspace, t_min, closet_t);
        if (hit_modelspace) {
            closet_hit_modelspace = hit_modelspace;
            closet_instance = &instance;
            closet_t = hit_modelspace->t;
        }
    }
    if (!closet_hit_modelspace) {
        return std::nullopt;
    }

    auto hit_point = glm::vec3{closet_instance->model2worldMat * glm::vec4{closet_hit_modelspace->hitPoint, 1}};
    auto hit_normal = glm::vec3{glm::transpose(closet_instance->world2modelMat) * glm::vec4{closet_hit_modelspace->hitNormal, 0}};
    auto hit_material = closet_instance->material ? closet_instance->material : closet_hit_modelspace->hitMaterial;

    return HitInfo{closet_t, hit_point, hit_normal, hit_material};
}

void Scene::addShape(const Shape &shape, const glm::vec3 &pos, const glm::vec3 &scale, const glm::vec3 &rotate, const Material *material) {
    glm::mat4 obj2worltMat =
        glm::translate(glm::mat4{1}, pos) *
        glm::rotate(glm::mat4{1}, glm::radians(rotate.z), {0, 0, 1}) *
        glm::rotate(glm::mat4{1}, glm::radians(rotate.y), {0, 1, 0}) *
        glm::rotate(glm::mat4{1}, glm::radians(rotate.x), {1, 0, 0}) *
        glm::scale(glm::mat4{1}, scale);
    addShape(shape, obj2worltMat, material);
}

void Scene::addShape(const Shape &shape, const glm::mat4 transMat, const Material *material) {
    std::optional<Bounds> bounds, boundsModelspace = shape.getBounds();
    if (boundsModelspace) {
        bounds = boundsModelspace->transformedBounds(transMat);
    }
    addShapeInstance(ShapeInstance{shape, transMat, glm::inverse(transMat), bounds, material});
}