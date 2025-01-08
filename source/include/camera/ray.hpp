#pragma once

#include <glm/glm.hpp>

class Ray {
  public:
    Ray(const glm::vec3 &_ori, const glm::vec3 &_dir) : origin{_ori}, direction{_dir} {}
    ~Ray() = default;

    glm::vec3 getOrigin() const { return origin; }
    glm::vec3 getDirection() const { return direction; }
  private:
    glm::vec3 origin, direction;
};

struct HitInfo {
    float t;
    glm::vec3 hitPoint, hitNormal;
};