#include "material/diffuse_material.hpp"
#include "util/global.hpp"

//TODO: whatever先应付一下
glm::vec3 DiffuseMaterial::sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo, glm::vec3 &beta) const {
    return albedo / M_PI;
}