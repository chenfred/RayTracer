#pragma once

#include "camera/camera.hpp"
#include "camera/light_sources.hpp"
#include "shape/scene.hpp"

class Renderer {
public:
    Renderer(Camera &_camera, const Scene &_scene, size_t concurrency) : camera{_camera}, scene{_scene}, threadPool{concurrency} {}
    void render(size_t spp, const std::filesystem::path &savePath);
    void render(const std::filesystem::path &savePath);

protected:
    Camera &camera;
    const Scene &scene;
    ThreadPool threadPool;
    RNG rng{static_cast<size_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())};

private:
    virtual glm::vec3 renderPixel(size_t x, size_t y) const = 0;
};

class PointLightRenderer : public Renderer {
public:
    PointLightRenderer(Camera &_cam, const Scene &_scene, size_t concurrency, const PointLight &_light)
        : Renderer(_cam, _scene, concurrency), light{_light} {}

protected:
    PointLight light;

private:
    virtual glm::vec3 renderPixel(size_t x, size_t y) const = 0;
};

class MultiPointLightsRenderer : public Renderer {
public:
    MultiPointLightsRenderer(Camera &_cam, const Scene &_scene, size_t concurrency, const std::vector<PointLight> &_lights)
        : Renderer(_cam, _scene, concurrency), pointLights{_lights} {}

protected:
    std::vector<PointLight> pointLights;

private:
    virtual glm::vec3 renderPixel(size_t x, size_t y) const = 0;
};

#define DEFINE_RENDERER(Name)                                                                                  \
    class Name : public Renderer {                                                                             \
    public:                                                                                                    \
        Name(Camera &camera, const Scene &scene, size_t concurrency) : Renderer(camera, scene, concurrency) {} \
                                                                                                               \
    private:                                                                                                   \
        glm::vec3 renderPixel(size_t x, size_t y) const override;                                              \
    };

#define DEFINE_POINT_LIGHT_RENDERER(Name)                                                                                     \
    class Name : public PointLightRenderer {                                                                                  \
    public:                                                                                                                   \
        Name(Camera &_cam, const Scene &_scene, size_t concurrency, const glm::vec3 &_light_pos, const glm::vec3 &_light_int) \
            : Name(_cam, _scene, concurrency, {_light_pos, _light_int}) {}                                                    \
        Name(Camera &_cam, const Scene &_scene, size_t concurrency, const PointLight &_light)                                 \
            : PointLightRenderer(_cam, _scene, concurrency, _light) {}                                                        \
                                                                                                                              \
    private:                                                                                                                  \
        glm::vec3 renderPixel(size_t x, size_t y) const override;                                                             \
    };

#define DEFINE_MULTI_POINT_LIGHTS_RENDERER(Name)                                                                              \
    class Name : public MultiPointLightsRenderer {                                                                            \
    public:                                                                                                                   \
        Name(Camera &_cam, const Scene &_scene, size_t concurrency, const glm::vec3 &_light_pos, const glm::vec3 &_light_int) \
            : Name(_cam, _scene, concurrency, {_light_pos, _light_int}) {}                                                    \
        Name(Camera &_cam, const Scene &_scene, size_t concurrency, const PointLight &_light)                                 \
            : Name(_cam, _scene, concurrency, std::vector<PointLight>{_light}) {}                                             \
        Name(Camera &_cam, const Scene &_scene, size_t concurrency, const std::vector<PointLight> &_lights)                   \
            : MultiPointLightsRenderer(_cam, _scene, concurrency, _lights) {}                                                 \
                                                                                                                              \
    private:                                                                                                                  \
        glm::vec3 renderPixel(size_t x, size_t y) const override;                                                             \
    };