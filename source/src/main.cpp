#include "camera/camera.hpp"
#include "camera/film.hpp"
#include "shape/sphere.hpp"
#include "thread/thread_pool.hpp"
#include "utils/timer.hpp"
#include <glm/glm.hpp>
#include <random>

void test_camera_ray_intersect();
void simple_test();
int main() {
    // simple_test();
    test_camera_ray_intersect();
    return 0;
}

void test_camera_ray_intersect(){
    size_t width = 1920, height = 1080;
    Film film{width, height};
    Sphere sphere{0.5f,glm::vec3{0}};
    Camera camera{film, {0,0,1}, {0,0,0}, 90};
    
    auto paint=[&film, &sphere, &camera](size_t x, size_t y)->void{
        auto eyeRay = camera.generateEyeRay({x,y});
        auto hitInfo = sphere.intersect(eyeRay);
        if(hitInfo.has_value()){
            film.setPixel(x, y, hitInfo->hitNormal);
        }
    };

    ThreadPool pool{128};
    pool.parallelFor(film.getWidth(), film.getHeight(), paint);

    film.save("sphere.png");
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

    ThreadPool thread_pool{};

    // Timer serialTimer{"serial setPixel"}; // 添加: 使用 Timer 对象进行计时
    // for (int x = 0; x < film.getWidth(); x++) {
    //     for (int y = 0; y < film.getHeight(); y++) {
    //         film.setPixel(x, y, randomColor);
    //     }
    // }
    // serialTimer.conclude();

    Timer parallel_timer{"parallel setPixel"};
    thread_pool.parallelFor(film.getWidth(), film.getHeight(),
                             [&film, &random_color](size_t x, size_t y) -> void {
                                 film.setPixel(x, y, random_color);
                             });
    parallel_timer.conclude();

    Timer save_timer("save to file");
    film.save("test.png");
    save_timer.conclude();
}