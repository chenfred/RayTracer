#pragma once

#include "camera/ray.hpp"

#include <cassert>
#include <format>
#include <iostream>
#include <limits>

class Bounds {
public:
    Bounds() : posMin(std::numeric_limits<float>::infinity()), posMax(-std::numeric_limits<float>::infinity()) {}
    Bounds(const glm::vec3 &_posMin, const glm::vec3 &_posMax);

    bool hasIntersection(const Ray &ray, float t_min, float t_max) const;
    Bounds transformedBounds(const glm::mat4 transMat) const;
    bool isValid() const { return posMin.x <= posMax.x && posMin.y <= posMax.y && posMin.z <= posMax.z; }
    glm::vec3 diagonal() const { return posMax - posMin; }
    glm::vec3 center() const { return (posMin + posMax) * 0.5f; }
    glm::vec3 corner(size_t index) const;

    void expand(const glm::vec3 &pos);
    void expand(const Bounds &bounds);

    // for debug
    void print() const { std::cout << std::format("Bounds: ({},{},{})->({},{},{})", posMin.x, posMin.y, posMin.z, posMax.x, posMax.y, posMax.z) << std::endl; }

private:
    glm::vec3 posMin, posMax;
};