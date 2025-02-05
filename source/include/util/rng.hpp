#pragma once

#include <glm/glm.hpp>
#include <random>
#include <chrono>

class RNG {
public:
    RNG() : RNG(static_cast<size_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())) {}
    RNG(size_t seed) { setSeed(seed); }

    // 生成[0, 1]的随机数
    float uniform() const { return uniform_distribution(gen); }
    // 生成[left, right]的随机数
    float uniform(float left, float right) const { return left + (right - left) * uniform(); };
    // 生成[left, right]的随机整数
    int uniformInteger(int left, int right) const { return static_cast<int>(uniform(left, right)); }
    // 概率prob返回true，概率1-prob返回false
    bool roulette(float prob) const { return uniform() < prob; }
    glm::vec3 uniformSampleHemisphere() const;
    glm::vec3 cosineSampleHemisphere() const;

    void setSeed(size_t seed) { gen.seed(seed); }

private:
    mutable std::mt19937 gen;
    mutable std::uniform_real_distribution<float> uniform_distribution{0, 1};
};