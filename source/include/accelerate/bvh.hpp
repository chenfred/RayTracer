#pragma once

#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "shape/shape.hpp"
#include "util/debug.hpp"

#include <array>
#include <cassert>
#include <format>
#include <optional>
#include <vector>

// FIXME: 导致模型漏三角形

template <typename T>
concept ShapeType = std::derived_from<T, Shape>;

template <ShapeType T>
struct BVHNode {
    Bounds bounds;
    std::vector<T> shapes;
    std::array<BVHNode<T> *, 2> children;

    void updateBounds() {
        bounds = {};
        for (const auto &shape : shapes) {
            std::optional<Bounds> b = shape.getBounds();
            if (b) {
                bounds.expand(b.value());
            }
        }
    }
};

template <ShapeType T>
class BVH : public Shape {
public:
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    std::optional<Bounds> getBounds() const override;
    void build(std::vector<T> &&shapes);

private:
    BVHNode<T> *root;

    void recursiveIntersect(BVHNode<T> *node, const Ray &ray, float t_min, float t_max, std::optional<HitInfo> &closestHit) const;
    void recursiveSplit(BVHNode<T> *node);
};

template <ShapeType T>
std::optional<HitInfo> BVH<T>::intersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closestHit;
    recursiveIntersect(root, ray, t_min, t_max, closestHit);
    return closestHit;
}

template <ShapeType T>
std::optional<Bounds> BVH<T>::getBounds() const {
    if (!root) {
        return {};
    }
    return root->bounds;
}

template <ShapeType T>
void BVH<T>::build(std::vector<T> &&shapes) {
    root = new BVHNode<T>{};
    root->shapes = std::move(shapes);
    root->updateBounds();
    recursiveSplit(root);
}

template <ShapeType T>
void BVH<T>::recursiveIntersect(BVHNode<T> *node, const Ray &ray, float t_min, float t_max, std::optional<HitInfo> &closestHit) const {
    if (!node || !node->bounds.hasIntersection(ray, t_min, t_max)) {
        return;
    }

    float closest_t = t_max;

    if (node->shapes.empty()) {
        // non-leafnode
        for (auto child : node->children) {
            std::optional<HitInfo> hit;
            recursiveIntersect(child, ray, t_min, closest_t, hit);
            if (hit && hit->t < closest_t) {
                closest_t = hit->t;
                closestHit = hit;
            }
        }
        return;
    }

    for (const auto &shape : node->shapes) {
        std::optional<HitInfo> hit = shape.intersect(ray, t_min, closest_t);
        if (hit) {
            closest_t = hit->t;
            closestHit = hit;
        }
    }
}

template <ShapeType T>
void BVH<T>::recursiveSplit(BVHNode<T> *node) {
    if (node->shapes.size() <= 1) {
        return;
    }

    const auto &bounds = node->bounds;
    auto diag = bounds.diagonal();
    size_t max_axis = diag.x > diag.y ? (diag.x > diag.z ? 0 : 2) : (diag.y > diag.z ? 1 : 2);
    float mid = bounds.center()[max_axis];

    std::array<std::vector<T>, 2> childShapes;
    for (const auto shape : node->shapes) {
        if (!shape.getBounds() || shape.getBounds().value().center()[max_axis] < mid) {
            childShapes[0].push_back(shape);
        } else {
            childShapes[1].push_back(shape);
        }
    }
    node->shapes.clear();
    node->shapes.shrink_to_fit();

    bool stopRecursion = false;
    if (childShapes[0].empty() || childShapes[1].empty()) {
        stopRecursion = true;
    }
    for (size_t i = 0; i < 2; ++i) {
        if (childShapes[i].empty()) {
            continue;
        }

        BVHNode<T> *child = new BVHNode<T>{};
        node->children[i] = child;
        child->shapes = childShapes[i];
        child->updateBounds();
        if (!stopRecursion) {
            recursiveSplit(child);
        }
    }
}
