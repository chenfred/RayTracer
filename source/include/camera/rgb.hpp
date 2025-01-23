#pragma once

#include <array>
#include <cstdint>
#include <glm/glm.hpp>

class RGB {
public:
    RGB(uint8_t r, uint8_t g, uint8_t b);
    RGB(const glm::ivec3 &color);
    RGB(const glm::vec3 &radiance);

    RGB blend(const RGB& rhs) const;
    glm::vec3 radiance() const;
    uint8_t r() const { return channels[0]; }
    uint8_t g() const { return channels[1]; }
    uint8_t b() const { return channels[2]; }
    std::array<uint8_t, 3> rgb() const { return channels; }


private:
    std::array<uint8_t, 3> channels;
};