#pragma once

#include <array>
#include <cstdint>
#include <glm/glm.hpp>

class RGB {
public:
    RGB(uint8_t r, uint8_t g, uint8_t b);
    RGB(const glm::ivec3 &color);
    RGB(const glm::vec3 &radiance);

    RGB blend(const RGB &rhs) const;
    glm::vec3 radiance() const;
    uint8_t r() const { return channels[0]; }
    uint8_t g() const { return channels[1]; }
    uint8_t b() const { return channels[2]; }
    std::array<uint8_t, 3> rgb() const { return channels; }

    static RGB Lerp(const RGB &r1, const RGB &r2, float t);
    static RGB GenerateHeatmapRGB(float t);

private:
    std::array<uint8_t, 3> channels;
};

static const std::array<RGB, 25> HEATMAP_COLOR_PANEL = {
    RGB{68, 1, 84},
    RGB{71, 17, 100},
    RGB{72, 31, 112},
    RGB{71, 45, 123},
    RGB{68, 58, 131},

    RGB{64, 70, 136},
    RGB{59, 82, 139},
    RGB{54, 93, 141},
    RGB{49, 104, 142},
    RGB{44, 114, 142},

    RGB{40, 124, 142},
    RGB{36, 134, 142},
    RGB{33, 144, 140},
    RGB{31, 154, 138},
    RGB{32, 164, 134},

    RGB{39, 173, 129},
    RGB{53, 183, 121},
    RGB{71, 193, 110},
    RGB{93, 200, 99},
    RGB{117, 208, 84},

    RGB{143, 215, 68},
    RGB{170, 220, 50},
    RGB{199, 224, 32},
    RGB{227, 228, 24},
    RGB{253, 231, 37},
};