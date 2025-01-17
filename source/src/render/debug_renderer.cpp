#include "renderer/debug_renderer.hpp"
#include "util/global.hpp"

glm::vec3 DebugInstanceRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hitInfo = scene.intersect(eyeRay);
    if (!hitInfo) {
        return {};
    }

    return get_predefined_radiance(hitInfo->hitInstance->index);
}
glm::vec3 DebugNormalRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hitInfo = scene.intersect(eyeRay);
    if (!hitInfo) {
        return {};
    }

    return hitInfo->hitNormal;
}

glm::vec3 DebugPositionRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hitInfo = scene.intersect(eyeRay);
    if (!hitInfo) {
        return {};
    }

    return hitInfo->hitPoint * 0.25f + 0.5f;
}

glm::vec3 DebugDepthRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hitInfo = scene.intersect(eyeRay);
    if (!hitInfo) {
        return {};
    }

    float depth = camera.getPosition().z - hitInfo->hitPoint.z;
    return glm::vec3(depth / 5);
}

glm::vec3 DebugLightDirRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hitInfo = scene.intersect(eyeRay);
    if (!hitInfo) {
        return {};
    }

    return -eyeRay.getDirection();
}