#include "renderer/simple_rt_renderer.hpp"
#include "camera/frame.hpp"

static const size_t NUM_LIGHTRAY_MAX_BOUNCES = 32;

glm::vec3 SimpleRayTracingRenderer::renderPixel(size_t x, size_t y) const {
    auto ray = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    glm::vec3 radiance{0}, beta{1};
    auto remaining_bounces = 1 + NUM_LIGHTRAY_MAX_BOUNCES; // EyeRay不算LightRay的弹射次数
    while (remaining_bounces--) {
        auto hit = scene.intersect(ray);
        if (!hit) {
            break;
        }

        // 只支持diffuse_material
        auto hit_info = hit.value();
        if(hit_info.hitMaterial->isEmitable()){
            radiance += beta * hit_info.hitMaterial->getEmissive();
            break;
        }
        LocalFrame frame{hit_info.hitNormal};
        auto wi_local = frame.toLocal(-ray.getDirection());
        auto wo_local = rng.uniformSampleHemisphere();
        radiance += hit_info.hitMaterial->sampleBSDF(wi_local, wo_local, beta);

        ray = Ray(hit_info.hitPoint, frame.toWorld(wo_local));
    }

    return radiance;
}