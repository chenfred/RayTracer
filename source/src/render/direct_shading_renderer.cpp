#include "renderer/direct_shading_renderer.hpp"

glm::vec3 DirectShadingRenderer::renderPixel(size_t x, size_t y) const {
    // casting ray
    auto eyeRay = camera.generateEyeRay({x, y});
    auto hitInfo = scene.intersect(eyeRay);
    if (!hitInfo) {
        return {};
    }
    const auto &point = hitInfo->hitPoint;
    const auto &normal = hitInfo->hitNormal;
    const auto *material = hitInfo->hitMaterial;
    const auto viewDir = -eyeRay.getDirection();

    // shading
    glm::vec3 radiance{};
    for (const auto &light : pointLights) {
        const auto lightDir = glm::normalize(light.position - point);
        const auto halfVector = glm::normalize(lightDir + viewDir);
        float dist = glm::distance(light.position, point);
        float dist2 = dist * dist;

        // specular term
        radiance += glm::vec3{0.5} * light.intensity *
                    std::pow(std::max(0.0f, glm::dot(halfVector, normal)), 128.0f) / dist2;
        // diffuse term
        glm::vec3 beta{1.0};
        radiance += material->sampleBSDF(-lightDir, viewDir, beta) * light.intensity * std::max(0.0f, glm::dot(lightDir, normal)) / dist;
        // ambient term
        radiance += glm::vec3{0.01} * light.intensity;
    }

    return radiance;
}