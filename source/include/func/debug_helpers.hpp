#pragma once

#include <format>
#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include <string_view>

#define ENABLE_DEBUG_PRINT

#ifdef ENABLE_DEBUG_PRINT
#define DEBUG_LINE(...) __VA_ARGS__ ;
#define DEBUG_PRINT(...) debug_print(__VA_ARGS__);
#define DEBUG_ASSERT(...) debug_assert(__VA_ARGS__);
#else
#define DEBUG_LINE(...)
#define DEBUG_PRINT(...)
#define DEBUG_ASSERT(...)
#endif

inline void debug_assert(bool condition){
#ifdef ENABLE_DEBUG_PRINT
    if(!condition){
        throw std::runtime_error("Assertion Failed!");
    }
#endif
}

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