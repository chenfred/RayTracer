#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "camera/rgb.hpp"
#include "material/basic_materials.hpp"
#include "renderer/debug_renderers.hpp"
#include "renderer/path_tracing_renderers.hpp"
#include "renderer/point_light_renderers.hpp"
#include "shape/cube.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/scene.hpp"
#include "shape/sphere.hpp"
#include "thread/thread_pool.hpp"
#include "util/globals.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static const size_t WIDTH = 1920;
static const size_t HEIGHT = 1080;

void test_path_tracing();
int main() {
    test_path_tracing();
    return 0;
}

void test_path_tracing() {
    // Colors & Materials
    RGB pink{255, 128, 128}, green{128, 255, 128}, blue{128, 128, 255}, white{255,255,255};
    RGB purple = pink.blend(blue), yellow = pink.blend(green), cyan = blue.blend(green);

    auto *emit_blue = new DiffuseMaterial(blue.radiance(), blue.radiance());
    auto *emit_pink = new DiffuseMaterial(pink.radiance(), pink.radiance());
    auto *emit_green = new DiffuseMaterial(green.radiance(), green.radiance());
    auto *emit_white = new DiffuseMaterial(white.radiance(), white.radiance());

    auto *diffuse_ground = new DiffuseMaterial(RGB(120, 204, 157).radiance());
    auto *diffuse_what = new DiffuseMaterial(RGB(202, 159, 117).radiance());

    auto *specular_full = new SpecularMaterial(glm::vec3{1.0});
    auto *specular_half = new SpecularMaterial(RGB(128, 128, 128).radiance());

    // shapes
    Model dragon{"resources/models/dragon/dragon_87k.obj"};
    Sphere sphere{1, {0, 0, 0}};
    Cube cube{1, {0, 0, 0}};
    Plane plane{{0, 0, 0}, {0, 1, 0}};

    // Camera
    Film film{WIDTH, HEIGHT};
    Camera camera(film, {0, 0, 2}, {0, 0, 0}, 90);

    // Scene
    Scene scene;
    scene.addShape(plane, diffuse_ground, {0, -0.25, 0});                                                  // 地面
    scene.addShape(sphere, emit_pink, {-1.5, 0, 0}, glm::vec3{0.5});         // 左边球
    scene.addShape(sphere, emit_blue, {1.5, 0, 0}, glm::vec3{0.5});          // 右边球
    scene.addShape(sphere, emit_green, {0, 0.25, -2}, glm::vec3{0.5});       // 前方球
    scene.addShape(sphere, specular_full, {1.25, 0.25, -1}, glm::vec3{0.5}); // 右前方镜面球
    scene.addShape(sphere, emit_white, {0, 0, 3}, glm::vec3{0.5});           // 相机后方球

    // scene.addShape(sphere, {0, 0, 0}, glm::vec3{0.125}, glm::vec3{0}, specular_full); //中间的小球
    scene.addShape(dragon, specular_full, {0, 0, 0}, {1, 1, 1}, {0, -90, 0});

    // ThreadPool
    ThreadPool *thread_pool = new ThreadPool{APP_CONCURRENCY};

    // Debug Info
    DebugInstanceRenderer(camera, scene)
        .render("./results/instance.png", thread_pool);
    DebugNormalRenderer(camera, scene)
        .render("./results/normal.png", thread_pool);

    // Render
    SimplePathTracingRenderer(camera, scene)
        .render(1, "./results/scene-simple-pt.png", thread_pool);
    WhittedRayTracingRenderer(camera, scene, {PointLight{{0, 1, 0}, glm::vec3{1}}})
        .render(1, "./results/scene-whitted-rt.png", thread_pool);
}