#include "util/global.hpp"

#include <array>
#include <thread>

const float M_PI = 3.1415926f;
const float FLOAT_CMP_EPS = 1e-5;

size_t INSTANCE_INDEX = 0;
size_t APP_CONCURRENCY = std::thread::hardware_concurrency();

static const std::array<glm::vec3, 16> PREDEFINED_RADIANCE_TABLE = {
    glm::vec3(1.0f, 0.0f, 0.0f), // Red
    glm::vec3(0.0f, 1.0f, 0.0f), // Green
    glm::vec3(0.0f, 0.0f, 1.0f), // Blue
    glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
    glm::vec3(1.0f, 0.0f, 1.0f), // Magenta
    glm::vec3(0.0f, 1.0f, 1.0f), // Cyan
    glm::vec3(1.0f, 0.5f, 0.0f), // Orange
    glm::vec3(0.5f, 0.0f, 0.5f), // Purple
    glm::vec3(0.0f, 0.5f, 0.5f), // Teal
    glm::vec3(0.5f, 0.5f, 0.5f), // Gray
    glm::vec3(1.0f, 1.0f, 1.0f), // White
    glm::vec3(0.0f, 0.0f, 0.0f), // Black
    glm::vec3(0.5f, 0.5f, 0.0f), // Olive
    glm::vec3(0.5f, 0.0f, 0.0f), // Maroon
    glm::vec3(0.0f, 0.5f, 0.0f), // Green
    glm::vec3(0.0f, 0.0f, 0.5f)  // Navy
};

glm::vec3 get_predefined_radiance(size_t index) {
    return PREDEFINED_RADIANCE_TABLE[index % 16];
}
