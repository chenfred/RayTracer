#include "shape/scene.hpp"
#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "shape/shape.hpp"

#include <cassert>
#include <glm/ext/matrix_transform.hpp>
#include <optional>

std::optional<HitInfo> Scene::intersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closestHit_modelspace;
    const ShapeInstance *closest_instance;
    float closestHitTime = t_max;
    for (const auto &instance : instances) {
        const auto bounds = instance.bounds;
        if (bounds && !bounds->hasIntersection(ray, t_min, closestHitTime)) {
            continue;
        }

        // 为了保持相交time在model空间和在world空间的一致可比较，这里的transformedRay不能对dir进行归一化：ray在transMat的作用下有可能被拉长或压缩
        const auto ray_modelspace = ray.transformedRay(instance.world2modelMat);
        const auto hit_modelspace = instance.shape.intersect(ray_modelspace, t_min, closestHitTime);
        if (hit_modelspace) {
            closestHitTime = hit_modelspace->t;
            closestHit_modelspace = hit_modelspace;
            closest_instance = &instance;
        }
    }
    if (!closestHit_modelspace) {
        return {};
    }

    const auto closestHitInfo_modelspace = closestHit_modelspace.value();
    auto hitPoint = glm::vec3(closest_instance->model2worldMat * glm::vec4{closestHitInfo_modelspace.hitPoint, 1});
    auto hitNormal = glm::normalize(glm::vec3{
        glm::transpose(closest_instance->world2modelMat) * glm::vec4{closestHitInfo_modelspace.hitNormal, 0}});
    auto hitMaterial = closest_instance->material ? closest_instance->material : closestHitInfo_modelspace.hitMaterial;
    return HitInfo{closestHitTime, hitPoint, hitNormal, hitMaterial, closest_instance};
}

void Scene::addShape(const Shape &shape, const glm::vec3 &pos, const glm::vec3 &scale, const glm::vec3 &rotate, const Material *material) {
    glm::mat4 model2worldMat =
        glm::translate(glm::mat4{1}, pos) *
        glm::rotate(glm::mat4{1}, glm::radians(rotate.z), {0, 0, 1}) *
        glm::rotate(glm::mat4{1}, glm::radians(rotate.y), {0, 1, 0}) *
        glm::rotate(glm::mat4{1}, glm::radians(rotate.x), {1, 0, 0}) *
        glm::scale(glm::mat4{1}, scale);
    addShape(shape, model2worldMat, material);
}

void Scene::addShape(const Shape &shape, const glm::mat4 model2worldMat, const Material *material) {
    std::optional<Bounds> bounds, boundsModelspace = shape.getBounds();
    if (boundsModelspace) {
        bounds = boundsModelspace->transformedBounds(model2worldMat);
    }
    addShapeInstance(ShapeInstance{shape, model2worldMat, glm::inverse(model2worldMat), bounds, material});
}