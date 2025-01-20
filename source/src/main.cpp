#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "glm/fwd.hpp"
#include "material/diffuse_material.hpp"
#include "renderer/debug_renderer.hpp"
#include "renderer/direct_shading_renderer.hpp"
#include "renderer/light.hpp"
#include "renderer/simple_rt_renderer.hpp"
#include "shape/cube.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/scene.hpp"
#include "shape/sphere.hpp"
#include "util/global.hpp"
#include "util/timer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static const size_t WIDTH = 1920;
static const size_t HEIGHT = 1080;

void test_renderer();
int main() {
    // APP_CONCURRENCY = 0;
    test_renderer();
    return 0;
}
void test_renderer() {
    // Point Light
    PointLight p1{{-0.5, 1, 1}, glm::vec3{2}};
    PointLight p2{{0.5, 1, -1}, glm::vec3{2}};
    std::vector<PointLight> plights{p1, p2};

    // glm::vec3 light_pos{0, 0.25, 0};
    // glm::vec3 light_intensity{3};

    // Material
    auto *grey_light = new DiffuseMaterial(glm::vec3{0.3});
    auto *grey = new DiffuseMaterial(glm::vec3{0.5});
    auto *blue = new DiffuseMaterial({0.3, 0.3, 0.7});
    auto *pink = new DiffuseMaterial({0.5, 0.1, 0.1});
    auto *emit = new DiffuseMaterial({0.3, 0.3, 0.7}, glm::vec3{0.1});

    // Scene
    Scene scene;

    // shapes
    // Model model{"resources/models/simple_dragon.obj", pink};
    Model model{"resources/models/dragon_87k.obj", pink};
    // Model model{"resources/models/dragon_871k.obj", pink};
    Sphere sphere{1, {0, 0, 0}, blue};
    Cube cube{1, {0, 0, 0}, blue};
    Plane plane{{0, 0, 0}, {0, 1, 0}, grey_light};
    scene.addShape(model, {0.5, 0, 0}, {1, 1, 1}, {0, -90, 0});
    // scene.addShape(sphere, {-0.5, 0, 0}, glm::vec3{0.25});
    scene.addShape(cube, {-0.5, 0, 0}, glm::vec3{0.25},{30,-45,45});
    scene.addShape(plane, {0, -0.25, 0});
    // scene.addShape(cube, {0, -0.5 - 0.25, 0}, {16, 1, 16}); // cube plane

    // Camera
    Film film{WIDTH, HEIGHT};
    Camera camera(film, {0, 0, 1.5}, {0, 0, 0}, 90);

    // Go!
    DirectShadingRenderer(camera, scene, APP_CONCURRENCY, plights)
        .render(1, "./results/scene.png");

    // scene.addShape(sphere, {-0.75, 0.5, 1.0}, glm::vec3{0.1}, glm::vec3{0}, emit); // 发光小球
    // SimpleRayTracingRenderer(camera, scene, APP_CONCURRENCY)
    //     .render(32, "./results/rt-scene.png");

    // Debug Go!
    // DebugInstanceRenderer(camera, scene, APP_CONCURRENCY)
    //     .render("./results/instance.png");
    // DebugNormalRenderer(camera, scene, APP_CONCURRENCY)
    //     .render("./results/normal.png");
    // DebugDepthRenderer(camera, scene, APP_CONCURRENCY)
    //     .render("./results/depth.png");

    // for (size_t i = 0; i < plights.size(); ++i) {
    //     const auto &light = plights[i];
    //     DebugVisibilityRenderer(camera, scene, APP_CONCURRENCY, light)
    //         .render(std::format("./results/visibility-light-{}.png", i));
    // }
}