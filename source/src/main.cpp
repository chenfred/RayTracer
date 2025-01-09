#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "shape/sphere.hpp"
#include "thread/thread_pool.hpp"
#include "utils/timer.hpp"
#include <cmath>
#include <format>
#include <glm/glm.hpp>
#include <iostream>
#include <random>

void test_camera_ray_intersect();
void simple_test();
int main() {
    // simple_test();
    test_camera_ray_intersect();
    return 0;
}

void test_camera_ray_intersect() {
    size_t width = 3840, height = 2560;
    Film film{width, height};
    glm::vec3 light_source_pos{2, 2, 2};
    glm::vec3 light_intensity{5};

    Sphere sphere{0.5f, glm::vec3{0}};
    Camera camera{film, {0, 0, 1}, {0, 0, -1}, 90};

    auto paint = [&](size_t x, size_t y) -> void {
        auto eyeRay = camera.generateEyeRay({x, y});
        auto hitInfo = sphere.intersect(eyeRay);
        if (!hitInfo.has_value())
            return;

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
    size_t width = 800, height = 600;
    glm::vec3 random_color{generateRandomNumber(), generateRandomNumber(), generateRandomNumber()};
    Film film{width, height};

    // Timer serialTimer{"serial setPixel"}; // 添加: 使用 Timer 对象进行计时
    // for (int x = 0; x < film.getWidth(); x++) {
    //     for (int y = 0; y < film.getHeight(); y++) {
    //         film.setPixel(x, y, randomColor);
    //     }
    // }
    // serialTimer.conclude();

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