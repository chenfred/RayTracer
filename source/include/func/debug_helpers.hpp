#pragma once

#include <format>
#include <iostream>
#include <string_view>
#include <glm/glm.hpp>

inline void debug_print(std::string_view msg){
    std::cout << "[DEBUG] " << msg << std::endl;
}

template <typename T, glm::qualifier Q>
inline void print_mat(const glm::mat<4, 4, T, Q> &m) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (j == 0) {
                std::cout << "[";
            }
            std::cout << std::format("{:.2f}", m[i][j]);
            if (j < 3) {
                std::cout << ", ";
            } else {
                std::cout << "]\n";
            }
        }
    }
    std::cout << std::endl;
}

template <typename T, glm::qualifier Q, int C, int R>
inline void print_mat(const glm::mat<C, R, T, Q> &m) {
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
}

template <typename T, glm::qualifier Q, int L>
inline void print_vec(const glm::vec<L, T, Q> &v) {
    std::cout << "[";
    for (int i = 0; i < L; ++i) {
        std::cout << std::format("{:.2f}", v[i]);
        if (i < L - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}