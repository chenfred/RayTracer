#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include "material/diffuse_material.hpp"
#include "shape/model.hpp"
#include "shape/plane.hpp"
#include "shape/sphere.hpp"
#include "thread/thread_pool.hpp"
#include "util/progress_bar.hpp"
#include "util/timer.hpp"


#include <cmath>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <vector>

void test_model();
void test_camera_ray_intersect();
void simple_test();

static const size_t WIDTH = 1280;
static const size_t HEIGHT = 720;

int main() {
    // simple_test();
    // test_camera_ray_intersect();
    test_model();
    return 0;
}

void test_model() {
    Film film{WIDTH, HEIGHT};
    glm::vec3 light_source_pos{-2, 2, 2};
    glm::vec3 light_intensity{5};

    // Material
    Material *diffuse_material = new DiffuseMaterial{glm::vec3{1}};
    
    // Shape
    // Model model{"resources/models/simple_dragon.obj"};
    Model model{"resources/models/dragon_87k.obj"};
    model.setMaterial(diffuse_material);
    auto transMat = glm::mat4{1.0f};
    transMat = glm::scale(transMat, glm::vec3{2});
    transMat = glm::rotate(transMat, glm::radians(90.0f), glm::vec3{0, 1, 0});
    model.applyTransform(transMat);
    Shape &shape{model};

    // Plane plane{{0, 0, 0}, glm::normalize(glm::vec3{-1, 1, 0.1}), diffuse_material};
    // Shape &shape{plane};

    // Camera
    Camera camera{film, {0, 0, 1}, {0, 0, -1}, 90};
    // Renderer
    ProgressBar progress_bar("Rendering");
    std::atomic<int> rendering_count = 0;
    auto paint = [&](size_t x, size_t y) -> void {
        rendering_count++;
        if (rendering_count % film.getWidth() == 0) {
            progress_bar.update(static_cast<double>(rendering_count) / (WIDTH * HEIGHT));
        }

        auto eyeRay = camera.generateEyeRay({x, y});
        auto hitInfo = shape.intersect(eyeRay);
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

        glm::vec3 color{};
        // specular term
        color += glm::vec3{0.5} * light_intensity *
                 std::pow(std::max(0.0f, glm::dot(halfVector, normal)), 128.0f) / dist;
        // diffuse term
        glm::vec3 beta{1.0};
        color += material->sampleBSDF(-lightDir, viewDir, beta) * light_intensity * std::max(0.0f, glm::dot(lightDir, normal)) / dist;
        // ambient term
        color += glm::vec3{0.01} * light_intensity;

        film.setPixel(x, y, color);
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

void test_camera_ray_intersect() {
    Film film{WIDTH, HEIGHT};
    glm::vec3 light_source_pos{-2, 2, 2};
    glm::vec3 light_intensity{5};

    Sphere sphere{0.5f, glm::vec3{0}};
    Triangle triangle{{-0.5, 0.5, 0}, {0.5, 0.5, 0}, {0.5, -0.5, 0}, {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}};
    Shape &shape{sphere};
    Camera camera{film, {0, 0, 1}, {0, 0, -1}, 90};

    ProgressBar progress_bar("Rendering");
    std::atomic<int> rendering_count = 0;
    auto paint = [&](size_t x, size_t y) -> void {
        rendering_count++;
        if (rendering_count % film.getWidth() == 0) {
            progress_bar.update(static_cast<double>(rendering_count) / (WIDTH * HEIGHT));
        }

        auto eyeRay = camera.generateEyeRay({x, y});
        auto hitInfo = shape.intersect(eyeRay);
        if (!hitInfo.has_value()) {
            return;
        }

        const auto &point = hitInfo->hitPoint;
        const auto &normal = hitInfo->hitNormal;
        auto lightDir = glm::normalize(light_source_pos - point);
        auto viewDir = -eyeRay.getDirection();
        auto halfVector = glm::normalize(lightDir + viewDir);
        float dist = glm::distance(light_source_pos, point);

        glm::vec3 color{};
        // specular term
        color += glm::vec3{0.5} * light_intensity *
                 std::pow(std::max(0.0f, glm::dot(halfVector, normal)), 128.0f) / dist;
        // diffuse term
        color += glm::vec3{0.3} * light_intensity * std::max(0.0f, glm::dot(lightDir, normal)) / dist;
        // ambient term
        color += glm::vec3{0.01} * light_intensity;

        film.setPixel(x, y, color);
    };

    Timer pool_rendering_timer("parallel for rendering");
    ThreadPool pool{};
    pool.parallelFor(film.getWidth(), film.getHeight(), paint);
    pool.wait();
    progress_bar.done();
    pool_rendering_timer.conclude();

    Timer save_film_timer("save film to file (may using ThreadPool)");
    film.save("sphere.png");
    save_film_timer.conclude();
}

float generateRandomNumber() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen);
}
void simple_test() {
    // size_t width = 800, height = 600;
    glm::vec3 random_color{generateRandomNumber(), generateRandomNumber(), generateRandomNumber()};
    Film film{WIDTH, HEIGHT};

    Timer parallel_timer{"parallel setPixel"};
    ThreadPool thread_pool{};
    thread_pool.parallelFor(film.getWidth(), film.getHeight(),
                            [&film, &random_color](size_t x, size_t y) -> void {
                                film.setPixel(x, y, random_color);
                            });
    thread_pool.wait();
    parallel_timer.conclude();

    Timer save_timer("save to file");
    film.save("test.png");
    save_timer.conclude();
}
