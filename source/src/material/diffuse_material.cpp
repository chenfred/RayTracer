#include "material/diffuse_material.hpp"
#include "util/global.hpp"
glm::vec3 DiffuseMaterial::sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo, glm::vec3 &beta) const {
    beta *= albedo;
    return albedo / M_PI;
}