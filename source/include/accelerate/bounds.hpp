#pragma once

#include "camera/ray.hpp"
#include "func/tools.hpp"

#include <cassert>
#include <format>
#include <iostream>
#include <limits>

class Bounds {
public:
    Bounds() : posMin{std::numeric_limits<float>::infinity()}, posMax{-std::numeric_limits<float>::infinity()} {}
    Bounds(const glm::vec3 &_posMin, const glm::vec3 &_posMax) : posMin{_posMin - BOUNDS_EDGE_EXPANDSION}, posMax{_posMax + BOUNDS_EDGE_EXPANDSION} {}

    bool hasIntersection(const Ray &ray, float t_min, float t_max) const { return hasIntersection(ray, 1.0f / ray.getDirection(), t_min, t_max); };
    bool hasIntersection(const Ray &ray, const glm::vec3 &rayDirInv, float t_min, float t_max) const;
    bool isValid() const { return vec_less_equal(posMin, posMax); }
    Bounds transformedBounds(const glm::mat4 transMat) const;
    glm::vec3 diagonal() const { return posMax - posMin; }
    glm::vec3 center() const { return (posMin + posMax) * 0.5f; }
    glm::vec3 corner(size_t index) const;
    float area() const;

    void expand(const glm::vec3 &pos);
    void expand(const Bounds &bounds);

    // for debug
    void print() const { std::cout << std::format("Bounds: ({},{},{})->({},{},{})", posMin.x, posMin.y, posMin.z, posMax.x, posMax.y, posMax.z) << std::endl; }

private:
    glm::vec3 posMin, posMax;

    // 用于防止bounds降维
    static constexpr float BOUNDS_EDGE_EXPANDSION = FLOAT_CMP_EPS;
};