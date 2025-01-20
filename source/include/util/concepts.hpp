#pragma once

#include <concepts>

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

class Shape;
template <typename T>
concept ShapeType = std::derived_from<T, Shape>;