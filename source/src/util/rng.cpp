#include "util/rng.hpp"
#include "util/globals.hpp"

glm::vec3 RNG::uniformSampleHemisphere() const {
    glm::vec3 result;
    do {
        result = {uniform(-1, 1), uniform(-1, 1), uniform(-1, 1)};
    } while (glm::length(result) > 1);
    if (result.y < 0) {
        result.y = -result.y;
    }
    return glm::normalize(result);
}

glm::vec3 RNG::cosineSampleHemisphere() const {
    float r = glm::sqrt(uniform());   // [0,1]
    float phi = 2 * M_PI * uniform(); // [0,2PI]
    return {r * glm::cos(phi), glm::sqrt(1 - r * r), r * glm::sin(phi)};
}