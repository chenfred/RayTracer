#pragma once

#include "util/global.hpp"

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template <typename T>
inline bool in_range(T value, T low, T high) {
    return value >= low && value <= high;
}

template <FloatingPoint T>
inline bool fequal(T left, T right, double EPS = FLOAT_CMP_EPS) {
    return fabs(left - right) < EPS;
}