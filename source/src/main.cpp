#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "camera/frame.hpp"
#include "camera/light_sources.hpp"
#include "func/debug_helpers.hpp"
#include "glm/geometric.hpp"
#include "material/basic_materials.hpp"
#include "renderer/debug_renderers.hpp"
#include "renderer/point_light_renderers.hpp"
#include "shape/cube.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/scene.hpp"
#include "shape/sphere.hpp"
#include "util/globals.hpp"

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
    std::vector<PointLight> plights{
        PointLight{{-0.5, 1, 0}, glm::vec3{2}},
        PointLight{{0.75, 1, 1}, glm::vec3{2}},
    };

    // Material
    auto *diffuse_lgrey = new DiffuseMaterial(glm::vec3{0.1});
    auto *diffuse_grey = new DiffuseMaterial(glm::vec3{0.2});
    auto *diffuse_blue = new DiffuseMaterial(glm::vec3{0.3, 0.3, 0.7});
    auto *diffuse_red = new DiffuseMaterial(glm::vec3{0.5, 0.1, 0.1});
    auto *specular = new SpecularMaterial(glm::vec3{1.0});

    // Scene
    Scene scene;

    // shapes
    Model dragon{"resources/models/dragon/dragon_87k.obj", diffuse_grey};
    Sphere sphere{1, {0, 0, 0}, specular};
    Cube cube{1, {0, 0, 0}, diffuse_grey};
    Plane x_plane{{0, 0, 0}, {1, 0, 0}, diffuse_red};
    Plane y_plane{{0, 0, 0}, {0, 1, 0}, diffuse_red};
    Plane z_plane{{0, 0, 0}, {0, 0, 1}, diffuse_red};

    scene.addShape(dragon, {0.5, 0, 0}, {1, 1, 1}, {0, -90, 0});
    scene.addShape(sphere, {-0.5, 0, 0}, glm::vec3{0.25});

    scene.addShape(y_plane, {0, -0.25, 0});                           // 地板
    scene.addShape(y_plane, {0, 1.25, 0}, glm::vec3{1}, {0, 0, 180}); // 屋顶
    scene.addShape(z_plane, {0, 0, -1});                              // 后方
    scene.addShape(z_plane, {0, 0, 2}, glm::vec3{1.0}, {180, 0, 0});  // 前方
    scene.addShape(x_plane, {-1, 0, 0});                              // 左侧
    scene.addShape(x_plane, {1, 0, 0}, glm::vec3{1}, {0, 180, 0});    // 右侧

    // Camera
    Film film{WIDTH, HEIGHT};
    Camera camera(film, {0, 0, 1.5}, {0, 0, 0}, 90);

    // Go!
    // DirectShadingRenderer(camera, scene, APP_CONCURRENCY, plights)
    //     .render(1, "./results/scene.png");
    WhittedRayTracingRenderer(camera, scene, APP_CONCURRENCY, plights)
        .render(16, "./results/scene-whitted-rt.png");

    // Debug Go!
    DebugInstanceRenderer(camera, scene, APP_CONCURRENCY)
        .render("./results/instance.png");
    DebugNormalRenderer(camera, scene, APP_CONCURRENCY)
        .render("./results/normal.png");
}