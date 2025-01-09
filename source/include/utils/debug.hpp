#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <format>

template <typename T, glm::qualifier Q>
void printMat(const glm::mat<4, 4, T, Q> &m) {
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
void printMat(const glm::mat<C, R, T, Q> &m) {
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
void printVec(const glm::vec<L, T, Q>& v) {
    std::cout << "[";
    for (int i = 0; i < L; ++i) {
        std::cout << std::format("{:.2f}", v[i]);
        if (i < L - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}