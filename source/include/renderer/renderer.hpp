#pragma once

#include "camera/camera.hpp"
#include "shape/scene.hpp"
#include "thread/thread_pool.hpp"
#include "util/rng.hpp"

#define DEFINE_RENDERER(Name)                                                                                            \
    class Name##Renderer : public Renderer {                                                                             \
    public:                                                                                                              \
        Name##Renderer(Camera &camera, const Scene &scene, size_t concurrency) : Renderer(camera, scene, concurrency) {} \
                                                                                                                         \
    private:                                                                                                             \
        glm::vec3 renderPixel(size_t x, size_t y) const override;                                                        \
    };

class Renderer {
public:
    Renderer(Camera &_camera, const Scene &_scene, size_t concurrency) : camera{_camera}, scene{_scene}, threadPool{concurrency} {}
    void render(size_t spp, const std::filesystem::path &savePath);
    void render(const std::filesystem::path &savePath);

protected:
    Camera &camera;
    const Scene &scene;
    ThreadPool threadPool;
    RNG rng{};

private:
    virtual glm::vec3 renderPixel(size_t x, size_t y) const = 0;
};

DEFINE_RENDERER(BlinnPhong)