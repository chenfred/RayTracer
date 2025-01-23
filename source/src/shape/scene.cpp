#include "shape/scene.hpp"
#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "func/graphics.hpp"
#include "shape/shape.hpp"

#include <cassert>
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <optional>

std::optional<HitInfo> Scene::intersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closestHit_modelspace;
    const ShapeInstance *closest_instance;
    float closestTime = t_max;
    for (const auto &instance : instances) {
        const auto bounds = instance.bounds;
        if (bounds.isValid() && !bounds.hasIntersection(ray, t_min, closestTime)) {
            continue;
        }

        // 为了保持相交time在model空间和在world空间的一致可比较，这里的transformedRay不能对dir进行归一化：ray在transMat的作用下有可能被拉长或压缩
        const auto ray_modelspace = ray.transformedRayAbnormalized(instance.world2modelMat);
        const auto hit_modelspace = instance.shape.intersect(ray_modelspace, t_min, closestTime);
        if (hit_modelspace) {
            closestTime = hit_modelspace->t;
            closestHit_modelspace = hit_modelspace;
            closest_instance = &instance;
        }
    }
    if (!closestHit_modelspace) {
        return {};
    }

    auto hitPoint = transformedPoint(closestHit_modelspace->hitPoint, closest_instance->model2worldMat);
    auto hitNormal = transformedNormal(closestHit_modelspace->hitNormal, closest_instance->world2modelMat);
    auto hitMaterial = closest_instance->material ? closest_instance->material : closestHit_modelspace->hitMaterial;
    return HitInfo{closestTime, hitPoint, hitNormal, hitMaterial, closest_instance};
}

std::optional<HitInfo> Scene::intersectTransformTime(const Ray &ray, float t_min, float t_max) const {
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
        float t_min_modelspace =
            ray_modelspace.hitAtTime(transformedPoint(ray.hitAtPoint(t_min), instance.world2modelMat));
        float t_max_modelspace =
            std::isinf(closestHitTime)
                ? closestHitTime
                : ray_modelspace.hitAtTime(transformedPoint(ray.hitAtPoint(closestHitTime), instance.world2modelMat));

        auto hit_modelspace = instance.shape.intersect(ray_modelspace, t_min_modelspace, t_max_modelspace);
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

void Scene::addShape(const Shape &shape, const Material *material, const glm::vec3 &pos, const glm::vec3 &scale, const glm::vec3 &rotate) {
    glm::mat4 model2worldMat =
        glm::translate(glm::mat4{1}, pos) *
        glm::rotate(glm::mat4{1}, glm::radians(rotate.z), {0, 0, 1}) *
        glm::rotate(glm::mat4{1}, glm::radians(rotate.y), {0, 1, 0}) *
        glm::rotate(glm::mat4{1}, glm::radians(rotate.x), {1, 0, 0}) *
        glm::scale(glm::mat4{1}, scale);
    addShape(shape, material, model2worldMat);
}

void Scene::addShape(const Shape &shape, const Material *material, const glm::mat4 model2worldMat) {
    Bounds bounds, boundsModelspace = shape.getBounds();
    if (boundsModelspace.isValid()) {
        bounds = boundsModelspace.transformedBounds(model2worldMat);
    }
    addShapeInstance(ShapeInstance{shape, model2worldMat, glm::inverse(model2worldMat), bounds, material});
}