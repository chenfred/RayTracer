#include "material/basic_materials.hpp"
#include "func/tools.hpp"
#include "glm/geometric.hpp"
#include "util/globals.hpp"

glm::vec3 DiffuseMaterial::sampleDirectionLocalized(const glm::vec3 &wi, const RNG &rng) const {
    return rng.cosineSampleHemisphere();
}

glm::vec3 DiffuseMaterial::sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo) const {
    return albedo; // TEST: 亮一点先
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

// FIXME: 折射算法有问题
glm::vec3 TransparentMaterial::sampleDirectionLocalized(const glm::vec3 &wi, const RNG &rng) const {
    // 法线方向
    constexpr glm::vec3 normal{0,1,0};

    // 计算入射角的余弦值
    float cosThetaI = glm::dot(wi, normal);

    // 判断光线是入射还是出射
    bool isEntering = cosThetaI >= 0;
    float etaI = isEntering ? 1.0f : refIndex; // 入射介质的折射率
    float etaT = isEntering ? refIndex : 1.0f; // 出射介质的折射率

    // 计算折射角的余弦值
    float sinThetaI = std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
    float sinThetaT = etaI / etaT * sinThetaI;

    // 判断是否发生全内反射
    if (sinThetaT >= 1.0f) {
        // 全内反射，返回反射方向
        return -glm::reflect(wi, normal);
    }

    // 计算折射方向
    glm::vec3 directionT = -etaI / etaT * wi + (etaI / etaT * cosThetaI - std::sqrt(1.0f - sinThetaT * sinThetaT)) * normal;

    // 归一化折射方向
    return glm::normalize(directionT);
}

// 添加 fresnelSchlick 函数
float fresnelSchlick(float cosTheta, float refIndex) {
    float r0 = (1.0f - refIndex) / (1.0f + refIndex);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * std::pow((1.0f - cosTheta), 5.0f);
}

glm::vec3 TransparentMaterial::sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo) const {
    // 计算入射角的余弦值
    float cosThetaI = glm::dot(wi, glm::vec3{0, 1, 0});

    // 计算折射方向
    glm::vec3 directionT = sampleDirectionLocalized(wi, RNG{});

    // 计算折射方向与出射方向的余弦值
    float cosThetaT = glm::dot(directionT, glm::vec3{0, 1, 0});

    // 计算菲涅尔透射系数
    float F = fresnelSchlick(cosThetaT, 1.0f / refIndex);

    // 计算透射率
    float eta = refIndex;
    float eta2 = eta * eta;
    float sin2ThetaT = eta2 * (1.0f - cosThetaI * cosThetaI);
    float cos2ThetaT = 1.0f - sin2ThetaT;
    float R = 0.5f * (eta2 - 1.0f + cos2ThetaT) / (eta2 + 1.0f + cos2ThetaT) + 0.5f * std::pow((eta * cosThetaI - std::sqrt(cos2ThetaT)) / (eta * cosThetaI + std::sqrt(cos2ThetaT)), 2);
    float T = 1.0f - R;

    // 返回透射率乘以 transmittance
    return T * transmittance;
}
