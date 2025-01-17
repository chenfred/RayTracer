#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "glm/fwd.hpp"
#include "material/diffuse_material.hpp"
#include "renderer/debug_renderer.hpp"
#include "renderer/direct_shading_renderer.hpp"
#include "renderer/simple_rt_renderer.hpp"
#include "shape/cube.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/scene.hpp"
#include "shape/shape.hpp"
#include "shape/sphere.hpp"
#include "shape/triangle.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void test_renderer();

static const size_t WIDTH = 400;
static const size_t HEIGHT = 300;

// TODO: 修一下一个球会被平面/像平面的长方体变得很奇怪的问题
int main() {
    test_renderer();
    return 0;
}
void test_renderer() {
    Film film{WIDTH, HEIGHT};
    glm::vec3 light_pos{0, 2, 2};
    glm::vec3 light_intensity{5};

    // shapes
    Sphere lightSphere{0.1, {0, 0, 0}, new DiffuseMaterial(glm::vec3{1.0}, glm::vec3{0.5})};
    Sphere sphere{1, {0, 0, 0}, new DiffuseMaterial(glm::vec3{0.3, 0.3, 0.7})};
    Cube cube{1, new DiffuseMaterial(glm::vec3{0.5})};
    Plane plane{{0, 0, 0}, {0, 1, 0}, new DiffuseMaterial({glm::vec3{0.1}})};
    Model model{"resources/models/simple_dragon.obj", new DiffuseMaterial(glm::vec3{0.3})};
    // Model model{"resources/models/dragon_87k.obj", new DiffuseMaterial(glm::vec3{0.3})};

    // Scene
    Scene scene;
    // scene.addShape(lightSphere, {0, 0.5, 0});
    scene.addShape(model, {0.75, 0, 0}, glm::vec3{1}, {0, -90, 0});
    scene.addShape(sphere, {-0.5, 0, 0.25}, glm::vec3{0.25});
    scene.addShape(cube, {0, -0.5, 0}, glm::vec3{16, 0.5, 16}, glm::vec3{0}); // 用作Plane的Cube
    // scene.addShape(plane, {0, -0.001, 0});

    // Camera
    Camera camera(film, {0, 0, 1}, {0, 0, 0}, 90);

    // Renderer
    DirectShadingRenderer directShadingRenderer(camera, scene, light_pos, light_intensity);
    SimpleRayTracingRenderer simpleRTRenderer(camera, scene);

    DebugInstanceRenderer instRen(camera, scene);
    DebugNormalRenderer normalRen(camera, scene);
    DebugDepthRenderer depthRen(camera, scene);
    DebugVisibilityRenderer visRen(camera, scene, light_pos, light_intensity);

    // Go!
    Renderer &renderer{directShadingRenderer};
    renderer.render(1, "./results/scene.png");

    // Debug Go!
    instRen.render("./results/instances.png");
    normalRen.render("./results/normal.png");
    depthRen.render("./results/depth.png");
    visRen.render("./results/visibility.png");
}