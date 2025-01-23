#include "camera/rgb.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

RGB::RGB(uint8_t r, uint8_t g, uint8_t b) {
    channels[0] = r;
    channels[1] = g;
    channels[2] = b;
}

RGB::RGB(const glm::ivec3 &color) {
    for (size_t i = 0; i < channels.size(); ++i) {
        channels[i] = static_cast<uint8_t>(color[i]);
    }
}

RGB::RGB(const glm::vec3 &radiance) {
    for (size_t i = 0; i < channels.size(); ++i) {
        channels[i] = static_cast<uint8_t>(std::clamp(std::pow(radiance[i], 1 / 2.2f) * 255.0f, 0.0f, 255.0f));
    }
}

RGB RGB::blend(const RGB &rhs) const {
    glm::ivec3 color{};
    for (size_t i = 0; i < channels.size(); ++i) {
        color[i] = (static_cast<int>(channels[i]) + static_cast<int>(rhs.channels[i])) / 2;
    }
    return RGB{color};
}

glm::vec3 RGB::radiance() const {
    glm::vec3 rad;
    for (size_t i = 0; i < channels.size(); ++i) {
        rad[i] = std::pow(channels[i] / 255.0f, 2.2f);
    }
    return rad;
}