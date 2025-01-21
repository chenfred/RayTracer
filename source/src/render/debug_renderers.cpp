#include "renderer/debug_renderers.hpp"
#include "util/globals.hpp"

glm::vec3 DebugInstanceRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }

    return get_predefined_radiance(hit->hitInstance->index);
}
glm::vec3 DebugNormalRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }

    return hit->hitNormal * 0.5f + 0.5f;
}

glm::vec3 DebugPositionRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }

    return hit->hitPoint;
}

glm::vec3 DebugDepthRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }

    float depth = camera.getPosition().z - hit->hitPoint.z;
    return glm::vec3(depth / 5);
}

glm::vec3 DebugLightDirRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }

    const auto &point = hit->hitPoint;
    const auto &normal = hit->hitNormal;
    const auto lightVec = light.position - point;
    const auto lightDir = glm::normalize(lightVec);

    return lightDir;
}

glm::vec3 DebugVisibilityRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }

    const auto &point = hit->hitPoint;
    const auto &normal = hit->hitNormal;
    const auto lightVec = light.position - point;
    const auto lightDir = glm::normalize(lightVec);

    if (glm::dot(lightDir, normal) < 0) {
        return {};
    }
    auto secondaryHit = scene.intersect(Ray{point, lightDir}, FLOAT_CMP_EPS, glm::length(lightVec));
    if (secondaryHit) {
        return {};
    }

    return glm::vec3{1.0};
}