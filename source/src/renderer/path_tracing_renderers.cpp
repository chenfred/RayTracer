#include "renderer/path_tracing_renderers.hpp"
#include "camera/frame.hpp"
#include "func/debug_helpers.hpp"
#include "glm/geometric.hpp"

static constexpr size_t LIGHTRAY_BOUNCES_LIMITED = 1024;
static constexpr float P_ROULETTE = 0.9f;
glm::vec3 SimplePathTracingRenderer::renderPixel(size_t x, size_t y) const {
    auto ray = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    glm::vec3 radiance{0}, beta{1};
    auto remaining_bounces = 1 + LIGHTRAY_BOUNCES_LIMITED;
    while (remaining_bounces) {
        auto hit = scene.intersect(ray);
        if (!hit) {
            break;
        }

        // 提取弹射点信息
        auto hitPoint = hit->hitPoint;
        auto hitNormal = hit->hitNormal;
        const Material *material = hit->hitMaterial;

        // 累加光源的辐射值
        radiance += beta * material->getEmissive();

        // 延迟的RR判定，有助于稳定光源亮度
        if (!rng.roulette(P_ROULETTE)) {
            break;
        }

        // 转换到法线的本地坐标系，方便计算
        LocalFrame frame{hitNormal};
        glm::vec3 wi_local = frame.toLocal(-ray.getDirection());

        // 衰减beta
        glm::vec3 wo_local;
        beta *= material->sampleBSDF(wi_local, rng, wo_local) / P_ROULETTE;

        // 继续发射光线
        ray = Ray(hitPoint, frame.toWorld(wo_local));
        remaining_bounces--;
    }

    DEBUG_PRINT("Hit the light ray bounces limit.", !remaining_bounces)

    return radiance;
}