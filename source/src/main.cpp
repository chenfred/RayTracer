#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "material/diffuse_material.hpp"
#include "renderer/direct_shading_renderer.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/scene.hpp"
#include "shape/sphere.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void test_renderer();

static const size_t WIDTH = 1920;
static const size_t HEIGHT = 1080;

int main() {
    test_renderer();
    return 0;
}

void test_renderer() {
    Film film{WIDTH, HEIGHT};
    glm::vec3 light_pos{0, 2, -2};
    glm::vec3 light_intensity{5};

    // Material
    Material *diffuse_material = new DiffuseMaterial{glm::vec3{1}};

    // Scene
    Model model{"resources/models/simple_dragon.obj", diffuse_material};
    // Model model{"resources/models/dragon_87k.obj", diffuse_material};
    Sphere sphere{0.5, {0, 0, 0}, diffuse_material};
    Plane plane{{0, 0, 0}, {0, 1, 0}, diffuse_material};

    Scene scene;
    scene.addShape(model, {-1, 0, -0.5}, glm::vec3{2});
    scene.addShape(sphere, {0, 0, 1.5}, glm::vec3{0.5});
    scene.addShape(plane, {0, -0.5, 0});

    // Camera
    Camera camera{film, {1.6, 0, 0}, {0, 0, 0}, 90};

    // Renderer
    DirectShadingRenderer directShadingRenderer{camera, scene, light_pos, light_intensity};
    Renderer &renderer{directShadingRenderer};

    // Go!
    renderer.render(32, "./result.png");
}