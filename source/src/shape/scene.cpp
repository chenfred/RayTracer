#include "shape/scene.hpp"
#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "util/timer.hpp"

#include <glm/ext/matrix_transform.hpp>

std::optional<HitInfo> Scene::intersect(const Ray &ray, float t_min, float t_max) const {
    return instanceBVH.intersect(ray, t_min, t_max);
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
    if(isBuilt()){
        throw std::runtime_error("Add shapes to scene after built is forbidden.");
    }

    Bounds bounds, boundsModelspace = shape.getBounds();
    if (boundsModelspace.isValid()) {
        bounds = boundsModelspace.transformedBounds(model2worldMat);
    }
    moveInstance(ShapeInstance{&shape, model2worldMat, glm::inverse(model2worldMat), bounds, material});
}

void Scene::build() {
    Timer buildTimer("Build instanceBVH for scene");
    instanceBVH.build(std::move(instances));
    buildTimer.conclude();
    std::vector<ShapeInstance>().swap(instances); // 清空容器
    built = true;
}