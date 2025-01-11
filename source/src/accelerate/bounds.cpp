#include "accelerate/bounds.hpp"
#include "glm/common.hpp"

// TODO: 有空再推一下
bool Bounds::hasIntersection(const Ray &ray, float t_min, float t_max) const {
    auto t1 = (posMin - ray.getOrigin()) / ray.getDirection();
    auto t2 = (posMax - ray.getOrigin()) / ray.getDirection();
    auto tmin = glm::min(t1, t2);
    auto tmax = glm::max(t1, t2);

    float near = std::max({tmin.x, tmin.t, tmin.z});
    float far = std::min({tmax.x, tmax.y, tmax.z});

    if (near < t_min && far > t_max) {
        return false;
    }

    return glm::min(far, t_max) > glm::max(near, t_min);
}

void Bounds::expand(const glm::vec3 &pos) {
    posMin = glm::min(posMin, pos);
    posMax = glm::max(posMax, pos);
}

void Bounds::expand(const Bounds &bounds) {
    posMin = glm::min(posMin, bounds.posMin);
    posMax = glm::max(posMax, bounds.posMax);
}

void Bounds::applyTransform(const glm::mat4 transMat) {
    posMin = glm::vec3{transMat * glm::vec4(posMin, 1.0f)};
    posMax = glm::vec3{transMat * glm::vec4(posMax, 1.0f)};
}