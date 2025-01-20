#pragma once

#include <glm/glm.hpp> 

constexpr float M_PI = 3.1415926f;
constexpr float FLOAT_CMP_EPS = 1e-5;

extern size_t INSTANCE_INDEX;
extern size_t APP_CONCURRENCY;

glm::vec3 get_predefined_radiance(size_t index);
