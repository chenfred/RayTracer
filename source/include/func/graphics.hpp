#pragma once

#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "util/concepts.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>

template <ShapeType T>
inline std::array<std::vector<T>, 2> split_shapes(std::vector<T> &&shapes, const std::function<bool(const T &, const T &)> &compare, float portion = 0.5f) {
    if (!in_range(portion, 0.0f, 1.0f)) {
        throw std::runtime_error("Split portion must between 0.0 and 1.0");
    }
    size_t splitPos = shapes.size() * portion;
    return split_shapes(std::move(shapes), compare, splitPos);
}

// 把[0,end)划分为[0, splitPos)和[splitPos, END)两个子集
template <ShapeType T>
inline std::array<std::vector<T>, 2> split_shapes(std::vector<T> &&shapes, const std::function<bool(const T &, const T &)> &compare, size_t splitPos) {
    // 使用std::nth_element来确保前indexMiddle个元素小于其余元素
    std::nth_element(shapes.begin(), shapes.begin() + splitPos, shapes.end(), compare);

    // 直接使用std::move避免不必要的拷贝
    return {std::vector<T>(std::make_move_iterator(shapes.begin()), std::make_move_iterator(shapes.begin() + splitPos)),
            std::vector<T>(std::make_move_iterator(shapes.begin() + splitPos), std::make_move_iterator(shapes.end()))};
}

inline glm::vec3 transformedPoint(const glm::vec3 &point, const glm::mat4 &transMat) {
    return glm::vec3{transMat * glm::vec4{point, 1}};
}

inline glm::vec3 transformedNormal(const glm::vec3 &normal, const glm::mat4 &invTransMat) {
    return glm::normalize(glm::vec3{
        glm::transpose(invTransMat) * glm::vec4{normal, 0}});
}