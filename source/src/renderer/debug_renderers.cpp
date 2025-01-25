#include "renderer/debug_renderers.hpp"
#include "camera/rgb.hpp"
#include "func/debug_helpers.hpp"
#include "util/globals.hpp"
#include <string>


glm::vec3 InstanceRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }

    return get_predefined_radiance(hit->hitInstance->index);
}
glm::vec3 NormalRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }

    return hit->hitNormal * 0.5f + 0.5f;
}

glm::vec3 PositionRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }

    return hit->hitPoint;
}

glm::vec3 DepthRenderer::renderPixel(size_t x, size_t y) const {
    auto eyeRay = camera.generateEyeRay({x, y}, {rng.uniform(), rng.uniform()});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }

    float depth = camera.getPosition().z - hit->hitPoint.z;
    return glm::vec3(depth / 10);
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

    return lightDir * 0.5f + 0.5f;
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
    auto secondaryHit = scene.intersect(Ray{point, lightDir}, FLOAT_LOOSE_POS_ZERO, glm::length(lightVec));
    if (secondaryHit) {
        return {};
    }

    return glm::vec3{1.0};
}

glm::vec3 DebugBoundsDepthRenderer::renderPixel(size_t x, size_t y) const {
#ifdef WITH_DEBUG_INFO
    auto eyeRay = camera.generateEyeRay({x, y});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }
    return RGB::GenerateHeatmapRGB(hit->boundsDepth / 32.0f).radiance();
#else
    return {};
#endif
}

glm::vec3 DebugBoundsTestCountRenderer::renderPixel(size_t x, size_t y) const {
#ifdef WITH_DEBUG_INFO
    auto eyeRay = camera.generateEyeRay({x, y});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }
    return RGB::GenerateHeatmapRGB(hit->boundsTestCount / 255.0f).radiance();
#else
    return {};
#endif
}

glm::vec3 DebugShapesTestCountRenderer::renderPixel(size_t x, size_t y) const {
#ifdef WITH_DEBUG_INFO
    auto eyeRay = camera.generateEyeRay({x, y});
    auto hit = scene.intersect(eyeRay);
    if (!hit) {
        return {};
    }
    return RGB::GenerateHeatmapRGB(hit->shapeTestCount / 25.5f).radiance();
#else
    return {};
#endif
}