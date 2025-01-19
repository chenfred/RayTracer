#pragma once

#include "light.hpp"
#include "renderer.hpp"

DEFINE_RENDERER(DebugInstance)

DEFINE_RENDERER(DebugNormal)

DEFINE_RENDERER(DebugPosition)

DEFINE_RENDERER(DebugDepth)

class DebugLightDirRenderer : public Renderer {
public:
    DebugLightDirRenderer(Camera &_cam, const Scene &_scene, size_t concurrency, const glm::vec3 &_light_pos, const glm::vec3 &_light_int)
        : DebugLightDirRenderer(_cam, _scene, concurrency, {_light_pos, _light_int}) {}
    DebugLightDirRenderer(Camera &_cam, const Scene &_scene, size_t concurrency, const PointLight &_light)
        : Renderer(_cam, _scene, concurrency), light{_light} {}

private:
    PointLight light;

    glm::vec3 renderPixel(size_t x, size_t y) const override;
};

class DebugVisibilityRenderer : public Renderer {
public:
    DebugVisibilityRenderer(Camera &_cam, const Scene &_scene, size_t concurrency, const glm::vec3 &_light_pos, const glm::vec3 &_light_int)
        : DebugVisibilityRenderer(_cam, _scene, concurrency, {_light_pos, _light_int}) {}
    DebugVisibilityRenderer(Camera &_cam, const Scene &_scene, size_t concurrency, const PointLight &_light)
        : Renderer(_cam, _scene, concurrency), light{_light} {}

private:
    PointLight light;

    glm::vec3 renderPixel(size_t x, size_t y) const override;
};