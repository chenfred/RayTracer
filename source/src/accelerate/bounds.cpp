#include "accelerate/bounds.hpp"

#include <cassert>
#include <glm/glm.hpp>
#include <stdexcept>

Bounds::Bounds(const glm::vec3 &_posMin, const glm::vec3 &_posMax)
    : posMin{_posMin - BOUNDS_EDGE_EXPANDSION}, posMax{_posMax + BOUNDS_EDGE_EXPANDSION} {
    // auto diff = posMax - posMin;
    // for (size_t i = 0; i < 3; ++i) {
    //     assert(diff[i] >= 0);
    //     if (diff[i] < FLOAT_CMP_EPS) {
    //         posMax[i] = posMin[i] + FLOAT_CMP_EPS;
    //     }
    // }
}

// TODO: 有空再推一下
bool Bounds::hasIntersection(const Ray &ray, float t_min, float t_max) const {
    if (!isValid()) {
        return false;
    }

    auto t1 = (posMin - ray.getOrigin()) / ray.getDirection();
    auto t2 = (posMax - ray.getOrigin()) / ray.getDirection();
    auto tmin = glm::min(t1, t2);
    auto tmax = glm::max(t1, t2);

    float near = std::max({tmin.x, tmin.t, tmin.z});
    float far = std::min({tmax.x, tmax.y, tmax.z});

    if (near <= t_min && far >= t_max) {
        return false;
    }

    return glm::min(far, t_max) >= glm::max(near, t_min);
}

void Bounds::expand(const glm::vec3 &pos) {
    posMin = glm::min(posMin, pos - BOUNDS_EDGE_EXPANDSION);
    posMax = glm::max(posMax, pos + BOUNDS_EDGE_EXPANDSION);
}

void Bounds::expand(const Bounds &bounds) {
    if (!bounds.isValid()) {
        return;
    }

    posMin = glm::min(posMin, bounds.posMin);
    posMax = glm::max(posMax, bounds.posMax);
}

Bounds Bounds::transformedBounds(const glm::mat4 transMat) const {
    if (!isValid()) {
        return {};
    }

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