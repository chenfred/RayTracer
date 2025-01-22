#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "camera/rgb.hpp"
#include "material/basic_materials.hpp"
#include "renderer/debug_renderers.hpp"
#include "renderer/path_tracing_renderers.hpp"
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

void test_path_tracing();
int main() {
    // APP_CONCURRENCY = 0;
    test_path_tracing();
    return 0;
}

void test_path_tracing() {
    // Material
    auto *emit_blue = new DiffuseMaterial(RGB(128, 128, 255).radiance(), {1, 1, 1});
    auto *emit_pink = new DiffuseMaterial(RGB(255, 128, 128).radiance(), {1, 1, 1});
    auto *emit_green = new DiffuseMaterial(RGB(128, 255, 128).radiance(), {1, 1, 1});
    auto *emit_white = new DiffuseMaterial(RGB(255, 255, 255).radiance(), {1, 1, 1});

    auto *diffuse_ground = new DiffuseMaterial(RGB(120, 204, 157).radiance());
    auto *diffuse_what = new DiffuseMaterial(RGB(202, 159, 117).radiance());

    auto *specular_full = new SpecularMaterial(glm::vec3{1.0});
    auto *specular_half = new SpecularMaterial(RGB(128, 128, 128).radiance());

    auto *glass = new TransparentMaterial(1.52, glm::vec3{1.0});

    // shapes
    Model dragon{"resources/models/dragon/simple_dragon.obj", diffuse_ground};
    Sphere sphere{1, {0, 0, 0}, diffuse_ground};
    Cube cube{1, {0, 0, 0}, specular_full};
    Plane plane{{0, 0, 0}, {0, 1, 0}, diffuse_ground};

    // Camera
    Film film{WIDTH, HEIGHT};
    Camera camera(film, {0, 0, 2}, {0, 0, 0}, 90);

    // Scene
    Scene scene;
    scene.addShape(plane, {0, -0.25, 0});                                               // 地面
    scene.addShape(sphere, {-1.5, 0, 0}, glm::vec3{0.5}, glm::vec3{0}, emit_pink);      // 左边球
    scene.addShape(sphere, {1.5, 0, 0}, glm::vec3{0.5}, glm::vec3{0}, emit_blue);       // 右边球
    scene.addShape(sphere, {0, 0.25, -2}, glm::vec3{0.5}, glm::vec3{0}, emit_green); // 前方球
    scene.addShape(sphere, {1.25, 0.25, -1}, glm::vec3{0.5}, glm::vec3{0}, specular_full); // 右前方镜面球
    scene.addShape(sphere, {0, 0, 3}, glm::vec3{0.5}, glm::vec3{0}, emit_white); // 相机后方球
    scene.addShape(sphere, {0, 0, 0}, glm::vec3{0.125}, glm::vec3{0}, specular_full); //中间的小球

    // Debug Info
    DebugInstanceRenderer(camera, scene, APP_CONCURRENCY)
        .render("./results/instance.png");
    DebugNormalRenderer(camera, scene, APP_CONCURRENCY)
        .render("./results/normal.png");

    // Render
    SimplePathTracingRenderer(camera, scene, APP_CONCURRENCY)
        .render(128, "./results/scene-simple-pt.png");
}