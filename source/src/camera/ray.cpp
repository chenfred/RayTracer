#include "camera/ray.hpp"

#include <glm/geometric.hpp>

Ray Ray::transformedRay(const glm::mat4 &transMat) const {
    glm::vec3 o = transMat * glm::vec4{origin, 1};
    glm::vec3 dir = glm::normalize(transMat * glm::vec4{direction, 0});
    return Ray{o, dir};
}