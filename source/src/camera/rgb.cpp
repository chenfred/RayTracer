#include "camera/rgb.hpp"
#include "func/tools.hpp"
#include "glm/common.hpp"
#include "util/globals.hpp"

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

RGB RGB::Lerp(const RGB &r1, const RGB &r2, float t) {
    glm::ivec3 color{};
    for (size_t i = 0; i < 3; ++i) {
        int ch1 = r1.channels[i];
        int ch2 = r2.channels[i];
        color[i] = std::clamp<int>(ch1 + (ch2 - ch1) * t, 0, 255);
    }
    return RGB{color};
}

RGB RGB::GenerateHeatmapRGB(float t) {
    if (!in_range(t, 0.0f, 1.0f - FLOAT_CMP_EPS)) {
        return RGB{255, 0, 0};
    }

    t *= HEATMAP_COLOR_PANEL.size() - 1;
    size_t index = std::floor(t);
    return Lerp(HEATMAP_COLOR_PANEL[index], HEATMAP_COLOR_PANEL[index + 1], glm::fract(t));
}