#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "camera/rgb.hpp"
#include "func/debug_helpers.hpp"
#include "glm/geometric.hpp"
#include "material/basic_materials.hpp"
#include "renderer/debug_renderers.hpp"
#include "renderer/path_tracing_renderers.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/scene.hpp"
#include "shape/sphere.hpp"
#include "thread/thread_pool.hpp"
#include "util/globals.hpp"

#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static const size_t WIDTH = 2560;
static const size_t HEIGHT = 1440;

void test_material() {
    auto *glass = new TransparentMaterial(1 / 1.33);
    RNG rng{};
    glm::vec3 wi, wo, out;

    wi = glm::normalize(glm::vec3{1, 0, 0});

    // wo=glm::refract(-wi,{0,1,0},1/1.33f);
    wo = glass->sampleDirectionLocalized(wi, rng);
    print_vec(wi);
    print_vec(wo);

    wi = wo;
    // wo=glm::refract(-wi,{0,-1,0},1.33f);
    wo = glass->sampleDirectionLocalized(wi, rng);
    print_vec(wi);
    print_vec(wo);
}

void test_path_tracing();
int main() {
    test_path_tracing();
    return 0;
}

void test_path_tracing() {
    // Colors & Materials
    RGB pink{255, 128, 128}, green{128, 255, 128}, blue{128, 128, 255}, white{255, 255, 255};
    RGB purple = pink.blend(blue), yellow = pink.blend(green), cyan = blue.blend(green);
    RGB bronze{205, 127, 50};

    auto *emit_blue = new DiffuseMaterial(blue.radiance(), glm::vec3{1});
    auto *emit_pink = new DiffuseMaterial(pink.radiance(), glm::vec3{1});
    auto *emit_green = new DiffuseMaterial(green.radiance(), glm::vec3{1});
    auto *emit_white = new DiffuseMaterial(white.radiance(), glm::vec3{1});
    auto *emit_purple = new DiffuseMaterial(purple.radiance(), glm::vec3{1});
    auto *emit_yellow = new DiffuseMaterial(yellow.radiance(), glm::vec3{1});
    auto *emit_cyan = new DiffuseMaterial(cyan.radiance(), glm::vec3{1});

    auto *diffuse_white = new DiffuseMaterial(white.radiance());

    auto *specular_full = new SpecularMaterial(glm::vec3{1.0});
    auto *specular_half = new SpecularMaterial(RGB(128, 128, 128).radiance());

    auto *glass = new TransparentMaterial(1 / 1.52);
    auto *reflective_glass = new TransparentMaterial(1 / 1.52, bronze.radiance(), 1.0f - bronze.radiance());

    // shapes
    Scene scene;
    Model dragon{"resources/models/dragon/dragon_871k.obj"};
    Sphere sphere{1, {0, 0, 0}};
    Plane plane{{0, 0, 0}, {0, 1, 0}};

    // Camera
    Film film{WIDTH, HEIGHT};
    Camera camera(film, {0, 0, 2}, {0, 0, 0}, 90);

    // Place to Scene
    scene.addShape(plane, diffuse_white, {0, -0.25, 0});                     // 地面
    scene.addShape(sphere, emit_purple, {-1.5, 0, 0}, glm::vec3{0.5});       // 左边球
    scene.addShape(sphere, emit_white, {0, 0.25, -2}, glm::vec3{0.5});      // 中后方球
    scene.addShape(sphere, emit_cyan, {1.5, 0, 0}, glm::vec3{0.5});          // 右边球
    scene.addShape(sphere, emit_white, {0, 0, 3}, glm::vec3{0.5});           // 相机后侧（前方）球
    scene.addShape(sphere, specular_full, {1.25, 0.25, -1}, glm::vec3{0.5}); // 右后方镜面球
    scene.addShape(dragon, reflective_glass, {0, 0, 0.5}, glm::vec3{1}, {0, -90, 0});

    // scene.addShape(sphere, glass, {0, 0, 0}, glm::vec3{0.125}, glm::vec3{0}); // 测材质专用：中间的小球
    // scene.addShape(dragon, glass, {0, 0, 0}, glm::vec3{1.5}, {0, 0, 0}); //测bvh专用

    // ThreadPool
    ThreadPool *thread_pool = new ThreadPool{APP_CONCURRENCY};

    // Debug Info
    DebugBoundsDepthRenderer(camera, scene)
        .render("./results/bounds-depth.png", thread_pool);
    DebugBoundsTestCountRenderer(camera, scene)
        .render("./results/bounds-test-count.png", thread_pool);
    DebugShapesTestCountRenderer(camera, scene)
        .render("./results/shapes-test-count.png", thread_pool);

    // Render
    SimplePathTracingRenderer(camera, scene)
        .render(128, "./results/scene-simple-pt.png", thread_pool);
}