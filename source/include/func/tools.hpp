#pragma once

#include "util/concepts.hpp"
#include "util/globals.hpp"

template <typename T>
inline bool in_range(T value, T low, T high) {
    return value >= low && value <= high;
}

template <FloatingPoint T>
inline bool fp_equal(T left, T right, double EPS = FLOAT_CMP_EPS) {
    return std::fabs(left - right) < EPS;
}

template <typename T, glm::qualifier Q, int L>
inline bool vec_equal(const glm::vec<L, T, Q> &v1, const glm::vec<L, T, Q> &v2, double EPS = FLOAT_CMP_EPS) {
    for (auto i = 0; i < L; ++i) {
        if (!fp_equal(v1[i], v2[i], EPS)) {
            return false;
        }
    }
    return true;
}

template <typename T, glm::qualifier Q, int L>
inline bool vec_less_equal(const glm::vec<L, T, Q> &v1, const glm::vec<L, T, Q> &v2, double EPS = FLOAT_CMP_EPS) {
    for (auto i = 0; i < L; ++i) {
        if (v1[i] > v2[i]) {
            return false;
        }
    }
    return true;
}