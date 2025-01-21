#pragma once

#include <glm/glm.hpp>
#include <random>

class RNG {
public:
    RNG(size_t seed) { setSeed(seed); }
    RNG() : RNG(0) {}

    // 生成[0, 1]的随机数
    float uniform() const { return uniform_distribution(gen); }
    // 生成[left, right]的随机数
    float uniform(float left, float right) const { return left + (right - left) * uniform(); };
    // 概率prob返回true，概率1-prob返回false
    bool roulette(float prob) const { return uniform() < prob; }
    glm::vec3 uniformSampleHemisphere() const;
    glm::vec3 cosineSampleHemisphere() const;

    void setSeed(size_t seed) { gen.seed(seed); }

private:
    mutable std::mt19937 gen;
    mutable std::uniform_real_distribution<float> uniform_distribution{0, 1};
};