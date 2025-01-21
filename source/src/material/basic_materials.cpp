#include "material/basic_materials.hpp"
#include "func/tools.hpp"
#include "util/globals.hpp"

glm::vec3 DiffuseMaterial::sampleDirectionLocalized(const glm::vec3 &wi, const RNG &rng) const {
    return rng.cosineSampleHemisphere();
}

glm::vec3 DiffuseMaterial::sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo) const {
    return albedo / M_PI;
}

glm::vec3 SpecularMaterial::sampleDirectionLocalized(const glm::vec3 &wi, const RNG &rng) const {
    return {-wi.x, wi.y, -wi.z};
}
glm::vec3 SpecularMaterial::sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo) const {
    if (!vequal(wo, {-wi.x, wi.y, -wi.z})) { // TODO: 仅支持在法线的Local坐标系进行判断
        return {};
    }
    return reflectance;
}