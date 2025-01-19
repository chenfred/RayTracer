#include "renderer/direct_shading_renderer.hpp"
#include "util/global.hpp"

#include <glm/geometric.hpp>

glm::vec3 DirectShadingRenderer::renderPixel(size_t x, size_t y) const {
    // casting ray
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }
    const auto &point = hit->hitPoint;
    const auto &normal = hit->hitNormal;
    const auto *material = hit->hitMaterial;
    const auto viewDir = -eyeRay.getDirection();

    // shading
    glm::vec3 radiance{};
    for (const auto &light : pointLights) {
        const auto lightVec = light.position - point;
        const auto lightDir = glm::normalize(lightVec);

        // For Debug Only
        glm::vec3 beta{1.0};
        glm::vec3 albedo_pi = material->sampleBSDF(-lightDir, viewDir, beta) * light.intensity;
        // ambient term
        radiance += albedo_pi * 0.01f * light.intensity;

        if (glm::dot(lightDir, normal) < 0) {
            continue;
        }
        auto shadowedHit = scene.intersect(Ray{point, lightDir}, FLOAT_CMP_EPS, glm::length(lightVec));
        if (shadowedHit) {
            continue;
        }

        const auto halfVector = glm::normalize(lightDir + viewDir);
        float dist = glm::distance(light.position, point);
        float dist2 = dist * dist;

        // specular term
        radiance += glm::vec3{0.5} * light.intensity *
                    std::pow(std::max(0.0f, glm::dot(halfVector, normal)), 128.0f) / dist2;
        // diffuse term
        radiance += albedo_pi * light.intensity * std::max(0.0f, glm::dot(lightDir, normal)) / dist;
        // ambient term
        // radiance += 0.01f * light.intensity;
    }

    return radiance;
}