#pragma once

#include <concepts>
#include <optional>

class Shape;
template <typename T>
concept ShapeType = std::derived_from<T, Shape>;

class Bounds;
template <typename T>
concept HasGetBounds = requires(T t) {
    { t.getBounds() } -> std::same_as<Bounds>;  // getBounds 返回 Bounds 类型
};

class Ray;
class HitInfo;
template <typename T>
concept HasIntersect = requires(T t, const Ray& ray, float t_min, float t_max) {
    { t.intersect(ray, t_min, t_max) } -> std::same_as<std::optional<HitInfo>>;  // intersect 返回 std::optional<HitInfo>
};

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;