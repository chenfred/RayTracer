#pragma once

#include <glm/glm.hpp>

class Ray {
  public:
    Ray(const glm::vec3 &_ori, const glm::vec3 &_dir) : origin{_ori}, direction{_dir} {}
    ~Ray() = default;

  private:
    glm::vec3 origin, direction;
};