#include "renderer/path_tracing_renderers.hpp"
#include "camera/frame.hpp"
#include "func/debug_helpers.hpp"
#include "glm/geometric.hpp"

static constexpr size_t LIGHTRAY_BOUNCES_LIMITED = 1024;
static constexpr float P_ROULETTE = 0.95f;
glm::vec3 SimplePathTracingRenderer::renderPixel(size_t x, size_t y) const {
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
        auto wo_local = material->sampleDirectionLocalized(wi_local, rng);


        // FIXME: 这里存在奇怪的背向法线的弹射
        // debug_print("Encounter a transmittance.", glm::dot(wo_local, {0,1,0})<0);

        // 累加光源的辐射值
        radiance += beta * material->getEmissive() / P_ROULETTE;
        beta *= material->sampleBSDF(wi_local, wo_local);

        // 继续发射光线
        ray = Ray(hitPoint, frame.toWorld(wo_local));
        remaining_bounces--;
    }

    debug_print("Hit the light ray bounces limit.", !remaining_bounces);

    return radiance;
}