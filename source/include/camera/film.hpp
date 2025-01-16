#pragma once

#include "thread/thread_pool.hpp"

#include <filesystem>
#include <glm/glm.hpp>
#include <vector>

class Film {
public:
    Film(size_t _width, size_t _height);
    ~Film() = default;

    void save(const std::filesystem::path &path, ThreadPool *threadPool = nullptr) const;
    float getAspectRatio() const { return static_cast<float>(width) / height; }
    size_t getWidth() const { return width; }
    size_t getHeight() const { return height; }
    glm::vec3 getPixel(size_t x, size_t y) const { return pixels[y * width + x]; }

    void setPixel(size_t x, size_t y, const glm::vec3 &color) { pixels[y * width + x] = color; }
    void clear() { pixels.clear(); pixels.resize(width * height); }

private:
    const size_t width, height;
    std::vector<glm::vec3> pixels; // 范围是[0,1]^3，存放算好的三通道Radiance而非最后的Color

    void saveToPPM(const std::filesystem::path &path) const;
    void saveToPNG(const std::filesystem::path &path, ThreadPool *threadPool) const;
};
