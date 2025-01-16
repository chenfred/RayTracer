#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "material/diffuse_material.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/scene.hpp"
#include "shape/sphere.hpp"
#include "thread/thread_pool.hpp"
#include "util/progress_bar.hpp"
#include "util/timer.hpp"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void test_scene();

static const size_t WIDTH = 1920;
static const size_t HEIGHT = 1080;

int main() {
    test_scene();
    return 0;
}

void test_scene() {
    Film film{WIDTH, HEIGHT};
    glm::vec3 light_source_pos{0, 2, -2};
    glm::vec3 light_intensity{1};

    // Material
    Material *diffuse_material = new DiffuseMaterial{glm::vec3{1}};

    // Scene
    Model model{"resources/models/simple_dragon.obj", diffuse_material};
    // Model model{"resources/models/dragon_87k.obj", diffuse_material};
    Sphere sphere{0.5, {0, 0, 0}, diffuse_material};
    Plane plane{{0, 0, 0}, {0, 1, 0}, diffuse_material};

    Scene scene;
    // scene.addShape(model, {-0.5, 0, 0});
    // scene.addShape(model, {0.5, 0, 0});
    scene.addShape(model, {-1, 0, -0.5}, glm::vec3{2});
    scene.addShape(sphere, {0, 0, 1.5}, glm::vec3{0.5});
    scene.addShape(plane, {0, -0.5, 0});

    // Camera
    Camera camera{film, {1.6, 0, 0}, {0, 0, 0}, 90};
    // Renderer
    ProgressBar progress_bar("Rendering");
    std::atomic<int> rendering_count = 0;
    auto paint = [&](size_t x, size_t y) -> void {
        int num_finished_pixels = ++rendering_count;
        // casting ray
        auto eyeRay = camera.generateEyeRay({x, y});
        auto hitInfo = scene.intersect(eyeRay);
        if (!hitInfo) {
            return;
        }
        const auto &point = hitInfo->hitPoint;
        const auto &normal = hitInfo->hitNormal;
        const auto *material = hitInfo->hitMaterial;
        auto lightDir = glm::normalize(light_source_pos - point);
        auto viewDir = -eyeRay.getDirection();
        auto halfVector = glm::normalize(lightDir + viewDir);
        float dist = glm::distance(light_source_pos, point);
        float dist2 = dist * dist;
        // shading
        glm::vec3 color{};
        // specular term
        color += glm::vec3{0.5} * light_intensity *
                 std::pow(std::max(0.0f, glm::dot(halfVector, normal)), 128.0f) / dist2;
        // diffuse term
        glm::vec3 beta{1.0};
        color += material->sampleBSDF(-lightDir, viewDir, beta) * light_intensity * std::max(0.0f, glm::dot(lightDir, normal)) / dist;
        // ambient term
        color += glm::vec3{0.01} * light_intensity;
        // write to framebuffer
        film.setPixel(x, y, color);
        // progress bar
        if (num_finished_pixels % film.getWidth() == 0) {
            progress_bar.update(static_cast<double>(num_finished_pixels) / (WIDTH * HEIGHT));
        }
    };
    // Go!
    Timer pool_rendering_timer("parallel for rendering");
    ThreadPool pool{};
    pool.parallelFor(film.getWidth(), film.getHeight(), paint);
    pool.wait();
    progress_bar.done();
    pool_rendering_timer.conclude();
    // Save result
    Timer save_film_timer("save film to file (may using ThreadPool)");
    film.save("result.png");
    save_film_timer.conclude();
}