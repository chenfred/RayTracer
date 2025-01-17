#include "util/rng.hpp"

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