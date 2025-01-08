#include "camera/film.hpp"
#include "thread/thread_pool.hpp"
#include "utils/timer.hpp"
#include <glm/glm.hpp>
#include <random>

void simple_test();
int main() {
    simple_test();
    return 0;
}
float generateRandomNumber() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen);
}
void simple_test() {
    size_t width = 800, height = 600;
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

    Timer saveTimer("save to file");
    film.save("test.png");
    saveTimer.conclude();
}