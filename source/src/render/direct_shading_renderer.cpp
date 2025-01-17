#include "renderer/direct_shading_renderer.hpp"

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
        const auto lightDir = glm::normalize(light.position - point);

        // TODO: 修一下阴影的问题
        // // 检测光源是否被遮挡
        // Ray visRay(point, lightDir);
        // auto shadowedHit = scene.intersect(visRay);
        // if (shadowedHit && shadowedHit->t < glm::distance(point, light.position)) {
        //     return {};
        // }

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