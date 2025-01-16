#include "camera/film.hpp"
#include "camera/rgb.hpp"
#include "glm/exponential.hpp"
#include "thread/thread_pool.hpp"

#include <cstddef>
#include <format>
#include <fstream>
#include <stb_image_write.h>
#include <stdexcept>
#include <string>

Film::Film(size_t _width, size_t _height) : width{_width}, height{_height} {
    pixels.resize(width * height);
}

void Film::save(const std::filesystem::path &path) const {
    std::string ext = path.extension().string();
    if (ext == ".ppm") {
        saveToPPM(path);
    } else if (ext == ".png") {
        saveToPNG(path);
    } else {
        throw std::runtime_error(std::format("Format {} not implemented yet.", ext)); // 使用 std::format 进行字符串格式化
    }
}

void Film::saveToPPM(const std::filesystem::path &path) const {
    std::ofstream file(path, std::ios::binary);
    file << "P6\n"
         << width << " " << height << "\n255\n";

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            RGB rgb{getPixel(x, y)};
            for (const auto channel : rgb.rgb()) {
                file << channel;
            }
            // const auto &color = getPixel(x, y);
            // glm::ivec3 icolor = glm::clamp(color * 255.0f, 0.0f, 255.0f);
            // for (auto i = 0; i < 3; ++i) {
            //     file << static_cast<uint8_t>(icolor[i]);
            // }
        }
    }
}

void Film::saveToPNG(const std::filesystem::path &path) const {
    // 将文件名从宽字符转换为 UTF-8 编码
    auto u8path = path.u8string();
    const char *utf8Path = reinterpret_cast<const char *>(u8path.data());

    // 分配内存存储像素数据，包括alpha通道
    std::vector<unsigned char> pixelData(width * height * 4);
    // 实现并发写入pixelData的函数
    auto f_loadingPixelData = [&](size_t y, size_t x) -> void {
        size_t index = (y * width + x) * 4;

        RGB color{getPixel(x, y)};
        for (const auto channel : color.rgb()) {
            pixelData[index] = channel;
            index++;
        }
        pixelData[index] = 255; // alpha通道设置为1.0
        // const auto &color = getPixel(x, y);
        // glm::ivec3 icolor = glm::clamp(color * 255.0f, 0.0f, 255.0f);
        // size_t index = (y * width + x) * 4;
        // pixelData[index + 0] = static_cast<uint8_t>(icolor[0]);
        // pixelData[index + 1] = static_cast<uint8_t>(icolor[1]);
        // pixelData[index + 2] = static_cast<uint8_t>(icolor[2]);
        // pixelData[index + 3] = 255; // alpha通道设置为1.0
    };
    // 用线程池执行并发写入
    ThreadPool pool{};
    pool.parallelFor(height, width, f_loadingPixelData);
    pool.wait();

    // 使用 stbi_write_png 保存 PNG 文件，指定通道数为4
    int result = stbi_write_png(utf8Path, static_cast<int>(width), static_cast<int>(height), 4, pixelData.data(), 0);
    if (result == 0) {
        throw std::runtime_error("Failed to save PNG file.");
    }
}