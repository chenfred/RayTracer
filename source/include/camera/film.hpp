#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <vector>

class Film {
  public:
    Film(size_t _width, size_t _height);

    void save(const std::filesystem::path &path);

    size_t getWidth() const { return width; }
    size_t getHeight() const { return height; }
    glm::vec3 getPixel(size_t x, size_t y) { return pixels.at(y * width + x); }
    void setPixel(size_t x, size_t y, const glm::vec3 &color) { pixels.at(y * width + x) = color; }

  private:
    size_t width, height;
    std::vector<glm::vec3> pixels;

    void saveToPPM(const std::filesystem::path &path);
    void saveToPNG(const std::filesystem::path &path);
};