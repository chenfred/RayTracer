#include "camera/film.hpp"
#include <cstddef>
#include <fstream>

Film::Film(size_t _width, size_t _height) : width{_width}, height{_height} {
    pixels.resize(width * height);
}

void Film::save(const std::filesystem::path &path) {
    // PPM
    // P3
    //<width> <height>
    // 255
    // rgb values -> ...
    std::ofstream file(path, std::ios::binary);
    file << "P6\n"
         << width << " " << height << "\n255\n";

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            const auto &color = getPixel(x, y);
            glm::ivec3 icolor = glm::clamp(color * 255.0f, 0.0f, 255.0f);
            for (auto i = 0; i < 3; ++i) {
                file << static_cast<uint8_t>(icolor[i]);
            }
        }
    }
}