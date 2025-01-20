#include "shape/scene.hpp"
#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "shape/shape.hpp"

#include <cassert>
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <optional>

std::optional<HitInfo> Scene::intersect(const Ray &ray, float t_min, float t_max) const {
    float closestHitTime = t_max;
    const ShapeInstance *closest_instance;
    glm::vec3 closestHitPoint;
    std::optional<HitInfo> closestHit_modelspace;
    for (auto &instance : instances) {
        if (instance.bounds.isValid() && !instance.bounds.hasIntersection(ray, t_min, closestHitTime)) {
            continue;
        }

        auto ray_modelspace = ray.transformedRay(instance.world2modelMat);
        // 需要将相交time先从worldspace转到modelspace
        auto closestHitTime_modelspace = std::isinf(closestHitTime) ? closestHitTime
                                                                    : ray_modelspace.hitAtTime(ray.hitAtPoint(closestHitTime)); 
        auto hit_modelspace = instance.shape.intersect(ray_modelspace, t_min, closestHitTime);
        if (!hit_modelspace) {
            continue;
        }

        // 再将相交time从modelspace转回worldspace
        auto hitPoint = glm::vec3{instance.model2worldMat * glm::vec4{hit_modelspace->hitPoint, 1}};
        auto hitTime = ray.hitAtTime(hitPoint);
        if (hitTime < closestHitTime) {
            closestHitTime = hitTime;
            closest_instance = &instance;
            closestHitPoint = hitPoint;
            closestHit_modelspace = hit_modelspace;
        }
    }
    if (!closestHit_modelspace) {
        return {};
    }

    auto normal = glm::normalize(glm::vec3{
        glm::transpose(closest_instance->world2modelMat) * glm::vec4{closestHit_modelspace->hitNormal, 0}});
    const Material *material = closest_instance->material ? closest_instance->material : closestHit_modelspace->hitMaterial;
    return HitInfo{closestHitTime, closestHitPoint, normal, material, closest_instance};
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
    Bounds bounds, boundsModelspace = shape.getBounds();
    if (boundsModelspace.isValid()) {
        bounds = boundsModelspace.transformedBounds(model2worldMat);
    }
    addShapeInstance(ShapeInstance{shape, model2worldMat, glm::inverse(model2worldMat), bounds, material});
}