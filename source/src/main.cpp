#include "camera/film.hpp"
#include "thread/thread_pool.hpp"
#include "utils/timer.hpp" // 添加: 包含 Timer 头文件
#include <glm/glm.hpp>
#include <random>

float generateRandomNumber() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen);
}

int main() {
    size_t width = 2560, height = 1440;
    glm::vec3 randomColor{generateRandomNumber(), generateRandomNumber(), generateRandomNumber()};
    Film film{width, height};

    ThreadPool thread_pool{};

    // Timer serialTimer{"serial setPixel"}; // 添加: 使用 Timer 对象进行计时
    // for (int x = 0; x < film.getWidth(); x++) {
    //     for (int y = 0; y < film.getHeight(); y++) {
    //         film.setPixel(x, y, randomColor);
    //     }
    // }
    // serialTimer.conclude();

    Timer parallelTimer{"parallel setPixel"};
    thread_pool.parallel_for(film.getWidth(), film.getHeight(),
                             [&film, &randomColor](size_t x, size_t y) -> void {
                                 film.setPixel(x, y, randomColor);
                             });
    parallelTimer.conclude();
    
    film.save("test.ppm");


    return 0;
}