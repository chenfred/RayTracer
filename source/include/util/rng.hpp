#pragma once

#include <random>
#include <glm/glm.hpp>

class RNG {
public:
    RNG(size_t seed) { setSeed(seed); }
    RNG() : RNG(0) {}

    float uniform() const { return uniform_distribution(gen); }
    float uniform(float left, float right) const { return left + (right - left) * uniform(); };
    glm::vec3 uniformSampleHemisphere() const;

    void setSeed(size_t seed) { gen.seed(seed); }

private:
    mutable std::mt19937 gen;
    mutable std::uniform_real_distribution<float> uniform_distribution{0, 1};
};