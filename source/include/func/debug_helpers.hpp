#pragma once

#include <format>
#include <glm/glm.hpp>
#include <iostream>
#include <string_view>

#define ENABLE_DEBUG_PRINT

inline void debug_print(std::string_view msg, bool condition = true) {
#ifdef ENABLE_DEBUG_PRINT
    if (condition) {
        std::cout << "[DEBUG] " << msg << std::endl;
    }
#endif
}

template <typename T, glm::qualifier Q, int C, int R>
inline void print_mat(const glm::mat<C, R, T, Q> &m) {
#ifdef ENABLE_DEBUG_PRINT
    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            if (j == 0) {
                std::cout << "[";
            }
            std::cout << std::format("{:.2f}", m[i][j]);
            if (j < C - 1) {
                std::cout << ", ";
            } else {
                std::cout << "]\n";
            }
        }
    }
    std::cout << std::endl;
#endif
}

template <typename T, glm::qualifier Q, int L>
inline void print_vec(const glm::vec<L, T, Q> &v) {
#ifdef ENABLE_DEBUG_PRINT
    std::cout << "[";
    for (int i = 0; i < L; ++i) {
        std::cout << std::format("{:.2f}", v[i]);
        if (i < L - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
#endif
}