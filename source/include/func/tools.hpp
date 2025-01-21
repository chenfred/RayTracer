#pragma once

#include "util/concepts.hpp"
#include "util/globals.hpp"

#include <stdexcept>
#include <algorithm>

template <typename T>
inline bool in_range(T value, T low, T high) {
    return value >= low && value <= high;
}

template <FloatingPoint T>
inline bool fequal(T left, T right, double EPS = FLOAT_CMP_EPS) {
    return fabs(left - right) < EPS;
}

template <typename T, glm::qualifier Q, int L>
inline bool vequal(const glm::vec<L, T, Q> &v1, const glm::vec<L, T, Q> &v2, double EPS = FLOAT_CMP_EPS) {
    for (auto i = 0; i < L; ++i) {
        if (!fequal(v1[i], v2[i], EPS)) {
            return false;
        }
    }
    return true;
}

template <typename T, glm::qualifier Q, int L>
inline bool vless(const glm::vec<L, T, Q> &v1, const glm::vec<L, T, Q> &v2, double EPS = FLOAT_CMP_EPS) {
    for (auto i = 0; i < L; ++i) {
        if (v1[i] > v2[i]) {
            return false;
        }
    }
    return true;
}

template <ShapeType T>
std::array<std::vector<T>, 2> split_shapes(std::vector<T> &&shapes, const std::function<bool(const T &, const T &)> &compare, float portion = 0.5f) {
    if (!in_range(portion, 0.0f, 1.0f)) {
        throw std::runtime_error("Split portion must between 0.0 and 1.0");
    }

    size_t indexMiddle = shapes.size() * portion;
    // 使用std::nth_element来确保前indexMiddle个元素小于其余元素
    std::nth_element(shapes.begin(), shapes.begin() + indexMiddle, shapes.end(), compare);

    // 直接使用std::move避免不必要的拷贝
    return {std::vector<T>(std::make_move_iterator(shapes.begin()), std::make_move_iterator(shapes.begin() + indexMiddle)),
            std::vector<T>(std::make_move_iterator(shapes.begin() + indexMiddle), std::make_move_iterator(shapes.end()))};
}