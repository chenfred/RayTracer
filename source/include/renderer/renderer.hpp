#pragma once

#include "camera/camera.hpp"
#include "shape/scene.hpp"
#include "thread/thread_pool.hpp"

#define DEFINE_RENDERER(Name) \
    class Name##Renderer : public Renderer { \
    public: \
        Name##Renderer(Camera &camera, const Scene &scene) : Renderer{camera, scene} {} \
    private: \
        glm::vec3 renderPixel(size_t x, size_t y) const override; \
    }; \

class Renderer {
public:
    Renderer(Camera &_camera, const Scene &_scene) : camera{_camera}, scene{_scene} {}
    void render(size_t spp, const std::filesystem::path &savePath);
    void render(const std::filesystem::path &savePath);

protected:
    Camera &camera;
    const Scene &scene;
    ThreadPool threadPool{};

private:
    virtual glm::vec3 renderPixel(size_t x, size_t y) const = 0;
};

DEFINE_RENDERER(BlinnPhong)