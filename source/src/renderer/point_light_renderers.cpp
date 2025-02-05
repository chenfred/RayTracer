#include "renderer/point_light_renderers.hpp"
#include "camera/frame.hpp"
#include "func/debug_helpers.hpp"
#include "glm/fwd.hpp"
#include "util/globals.hpp"

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

        glm::vec3 albedo = light.intensity * material->evalBSDF(-lightDir, viewDir);
        // accumulate ambient term ahead
        radiance += albedo * 0.01f * light.intensity; 

        if (glm::dot(lightDir, normal) < 0) {
            continue;
        }
        auto shadowedHit = scene.intersect(Ray{point, lightDir}, FLOAT_LOOSE_POS_ZERO, glm::length(lightVec));
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
        radiance += albedo * light.intensity * std::max(0.0f, glm::dot(lightDir, normal)) / dist;
    }

    return radiance;
}

static constexpr size_t LIGHTRAY_BOUNCES_LIMITED = 1024;
static constexpr float P_ROULETTE = 0.9f;
glm::vec3 WhittedRayTracingRenderer::renderPixel(size_t x, size_t y) const {
    auto ray = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    glm::vec3 radiance{0}, beta{1};
    auto remaining_bounces = 1 + LIGHTRAY_BOUNCES_LIMITED;
    while (remaining_bounces && rng.roulette(P_ROULETTE)) {
        auto hit = scene.intersect(ray);
        if (!hit) {
            break;
        }

        // 提取弹射点信息
        auto hitPoint = hit->hitPoint;
        auto hitNormal = hit->hitNormal;
        const Material *material = hit->hitMaterial;

        // 转换到法线的本地坐标系，方便计算
        LocalFrame frame{hitNormal};
        auto wi_local = frame.toLocal(-ray.getDirection());
        auto wo_local = material->sampleScatteringDirection(wi_local, rng);

        // 对当前弹射点进行点光源的直接着色
        glm::vec3 local_shading{};
        for (const auto &light : pointLights) {
            auto lightVec = light.position - hitPoint;
            auto lightVecLength = glm::length(lightVec);
            auto lightDir = lightVec / lightVecLength; // 手工normalize
            auto shadowedHit = scene.intersect(Ray{hitPoint, lightDir}, FLOAT_LOOSE_POS_ZERO, lightVecLength);
            if (shadowedHit) {
                continue;
            }

            auto dist = glm::distance(light.position, hitPoint);
            auto dist2 = dist * dist;

            auto wl_local = frame.toLocal(lightDir);
            local_shading += light.intensity * material->evalBSDF(wi_local, wl_local) * std::max(0.0f, glm::dot(wl_local, {0, 1, 0})); // diffuse term
            local_shading += light.intensity * glm::vec3{0.5} * std::pow(std::max(0.0f, glm::dot(glm::normalize(wi_local + wl_local), glm::vec3{0, 1, 0})), 128.0f) / dist;
        }

        // 累加当前弹射点的radiance贡献
        radiance += beta * (local_shading + material->getEmissive()) / P_ROULETTE;
        beta *= material->evalBSDF(wi_local, wo_local);

        // 继续发射光线
        ray = Ray(hitPoint, frame.toWorld(wo_local));
        remaining_bounces--;
    }

    DEBUG_PRINT("Hit the light ray bounces limit.", !remaining_bounces)

    return radiance;
}