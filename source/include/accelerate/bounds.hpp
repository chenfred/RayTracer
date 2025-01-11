#pragma once

#include "camera/ray.hpp"

#include <cassert>
#include <format>
#include <iostream>
#include <limits>

class Bounds {
public:
    Bounds() : posMin(std::numeric_limits<float>::infinity()), posMax(-std::numeric_limits<float>::infinity()) {}
    Bounds(const glm::vec3 &_posMin, const glm::vec3 &_posMax) : posMin(_posMin), posMax(_posMax) {}

    bool hasIntersection(const Ray &ray, float t_min, float t_max) const;

    void expand(const glm::vec3 &pos);
    void expand(const Bounds &bounds);
    void applyTransform(const glm::mat4 transMat);
    void print() {
        std::cout << std::format("Bounds: ({},{},{})->({},{},{})", posMin.x, posMin.y, posMin.z, posMax.x, posMax.y, posMax.z) << std::endl;
    }

private:
    glm::vec3 posMin, posMax;
};