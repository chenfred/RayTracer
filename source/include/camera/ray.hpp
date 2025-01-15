#pragma once

#include "material/material.hpp"
#include "util/debug.hpp"

#include <format>
#include <glm/glm.hpp>
#include <iostream>

class Ray {
public:
    Ray(const glm::vec3 &_ori, const glm::vec3 &_dir) : origin{_ori}, direction{_dir} {}
    ~Ray() = default;

    Ray transformedRay(const glm::mat4 &transMat) const;
    glm::vec3 hitAtPoint(float t) const { return origin + t * direction; }
    glm::vec3 getOrigin() const { return origin; }
    glm::vec3 getDirection() const { return direction; }

    // for debug
    void print() const { std::cout << std::format("Ray:[{},{},{}]->({},{},{})", origin.x, origin.y, origin.z, direction.x, direction.y, direction.z) << std::endl; }
    bool operator==(const Ray &rhs) const { return vequal(origin, rhs.origin, 1e-3) && vequal(direction, rhs.direction, 1e-4); }

private:
    glm::vec3 origin, direction;
};

struct HitInfo {
    float t;
    glm::vec3 hitPoint, hitNormal;
    const Material *hitMaterial{};
};
