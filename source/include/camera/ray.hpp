#pragma once

#include "func/tools.hpp"
#include "glm/geometric.hpp"
#include "material/material.hpp"


#include <format>
#include <glm/glm.hpp>
#include <iostream>

class Ray {
public:
    Ray(const glm::vec3 &_ori, const glm::vec3 &_dir) : origin{_ori}, direction{_dir} {}
    ~Ray() = default;

    // 变换一个Ray，然后归一化direction
    Ray transformedRay(const glm::mat4 &transMat) const;
    // 变换一个Ray，但不归一化direction
    Ray transformedRayAbnormalized(const glm::mat4 &transMat) const;
    float hitAtTime(const glm::vec3 &hitPoint) const { return glm::length(hitPoint - origin); }
    glm::vec3 hitAtPoint(float t) const { return origin + t * direction; }
    glm::vec3 getOrigin() const { return origin; }
    glm::vec3 getDirection() const { return direction; }

    // for debug
    void print() const { std::cout << std::format("Ray:[{},{},{}]->({},{},{})", origin.x, origin.y, origin.z, direction.x, direction.y, direction.z) << std::endl; }
    bool operator==(const Ray &rhs) const { return vequal(origin, rhs.origin) && vequal(direction, rhs.direction); }

private:
    glm::vec3 origin, direction;
};

struct ShapeInstance;

struct HitInfo {
    float t;
    glm::vec3 hitPoint, hitNormal;
    const Material *hitMaterial{};
    const ShapeInstance *hitInstance{};
};
