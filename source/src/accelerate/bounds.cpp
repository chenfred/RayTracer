#include "accelerate/bounds.hpp"
#include "glm/common.hpp"

#include <stdexcept>

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

Bounds Bounds::transformedBounds(const glm::mat4 transMat) const {
    Bounds bounds{};

    for (size_t index = 0; index < 8; ++index) {
        auto corner = this->corner(index);
        bounds.expand(glm::vec3(transMat * glm::vec4(corner, 1)));
    }
    return bounds;
}

glm::vec3 Bounds::corner(size_t index) const {
    if (index >= 8) {
        throw std::out_of_range("Bounds::corner(size_t index) index must less than 8.");
    }

    auto corner = posMax;
    // 用3个bit位来控制某个维度(x, y, z)为min或max
    if ((index & 0b001) == 0) {
        corner.x = posMin.x;
    }
    if ((index & 0b010) == 0) {
        corner.y = posMin.y;
    }
    if ((index & 0b100) == 0) {
        corner.z = posMin.z;
    }
    return corner;
}