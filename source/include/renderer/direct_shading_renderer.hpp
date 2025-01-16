#pragma once

#include "renderer.hpp"

struct PointLight {
    glm::vec3 position, intensity;
};

class DirectShadingRenderer : public Renderer {
public:
    DirectShadingRenderer(Camera &_cam, const Scene &_scene, const glm::vec3 &_light_pos, const glm::vec3 &_light_int)
        : DirectShadingRenderer(_cam, _scene, {_light_pos, _light_int}) {}
    DirectShadingRenderer(Camera &_cam, const Scene &_scene, const PointLight &_light)
        : DirectShadingRenderer(_cam, _scene, std::vector<PointLight>{_light}) {}
    DirectShadingRenderer(Camera &_cam, const Scene &_scene, const std::vector<PointLight> _lights)
        : Renderer(_cam, _scene), pointLights{_lights} {}

private:
    std::vector<PointLight> pointLights;

    glm::vec3 renderPixel(size_t x, size_t y) const override;
};
