#pragma once

#include "renderer.hpp"
#include "light.hpp"

class DirectShadingRenderer : public Renderer {
public:
    DirectShadingRenderer(Camera &_cam, const Scene &_scene, size_t concurrency,const glm::vec3 &_light_pos, const glm::vec3 &_light_int)
        : DirectShadingRenderer(_cam, _scene, concurrency, {_light_pos, _light_int}) {}
    DirectShadingRenderer(Camera &_cam, const Scene &_scene, size_t concurrency,const PointLight &_light)
        : DirectShadingRenderer(_cam, _scene, concurrency, std::vector<PointLight>{_light}) {}
    DirectShadingRenderer(Camera &_cam, const Scene &_scene, size_t concurrency, const std::vector<PointLight> _lights)
        : Renderer(_cam, _scene, concurrency), pointLights{_lights} {}

private:
    std::vector<PointLight> pointLights;

    glm::vec3 renderPixel(size_t x, size_t y) const override;
};