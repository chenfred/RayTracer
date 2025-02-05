#include "material/basic_materials.hpp"
#include "func/tools.hpp"
#include "glm/geometric.hpp"

glm::vec3 DiffuseMaterial::sampleScatteringDirection(const glm::vec3 &wi, const RNG &rng) const {
    return rng.cosineSampleHemisphere();
}

glm::vec3 DiffuseMaterial::evalBSDF(const glm::vec3 &wi, const glm::vec3 &wo) const {
    return albedo;
}

glm::vec3 SpecularMaterial::sampleScatteringDirection(const glm::vec3 &wi, const RNG &) const {
    return {-wi.x, wi.y, -wi.z};
}

glm::vec3 SpecularMaterial::evalBSDF(const glm::vec3 &wi, const glm::vec3 &wo) const {
    if (!vec_equal(wo, {-wi.x, wi.y, -wi.z})) { // TODO: 仅支持在法线的Local坐标系进行判断
        return {};
    }
    return reflectance;
}

glm::vec3 TransparentMaterial::sampleScatteringDirection(const glm::vec3 &wi, const RNG &) const {
    if (wi.y >= 0) { // 进入材质
        return glm::refract(-wi, {0, 1, 0}, eta);
    }

    // 离开材质
    auto ref = glm::refract(-wi, {0, -1, 0}, 1 / eta);
    if (vec_equal(ref, glm::vec3{0})) {
        return {-wi.x, wi.y, -wi.z};
    }
    return ref;
}

float TransparentMaterial::fresnelSchlick(float cosTheta, float refIndex) const {
    float r0 = (1.0f - refIndex) / (1.0f + refIndex);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * std::pow((1.0f - cosTheta), 5.0f);
}

glm::vec3 TransparentMaterial::evalBSDF(const glm::vec3 &wi, const glm::vec3 &wo) const {
    bool isReflect = wi.y * wo.y > 0;
    return isReflect ? reflectance : transmittance;
}
