#include "material/material.hpp"

glm::vec3 Material::sampleBSDF(const glm::vec3 &wi, const RNG &rng, glm::vec3 &wo) const {
    wo = sampleScatteringDirection(wi, rng);
    return evalBSDF(wi, wo);
}