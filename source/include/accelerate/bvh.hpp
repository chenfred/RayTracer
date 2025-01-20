#pragma once

#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "func/tools.hpp"
#include "shape/shape.hpp"
#include "util/concepts.hpp"

#include <array>
#include <cassert>
#include <optional>
#include <vector>

// TODO: 实现更高效的BVH（例如把递归转成迭代）
// FIXME: 修复导致dragon_871k.obj的三角形稀碎的问题

template <ShapeType T>
struct BVHNode {
    Bounds bounds;
    std::vector<T> shapes;
    std::array<BVHNode<T> *, 2> children;
    void updateBounds() {
        bounds = {};
        for (const T &shape : shapes) {
            bounds.expand(shape.getBounds());
        }
    }
};

template <ShapeType T>
class BVH : public Shape {
public:
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bounds getBounds() const override;

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
Bounds BVH<T>::getBounds() const {
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

    if (node->shapes.empty()) {
        // non-leafnode
        for (auto child : node->children) {
            std::optional<HitInfo> hit;
            recursiveIntersect(child, ray, t_min, t_max, hit);
            if (hit) {
                t_max = hit->t;
                closestHit = hit;
            }
        }
        return;
    }

    for (const auto &shape : node->shapes) {
        std::optional<HitInfo> hit = shape.intersect(ray, t_min, t_max);
        if (hit) {
            t_max = hit->t;
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
    size_t maxAxis = diag.x > diag.y ? (diag.x > diag.z ? 0 : 2) : (diag.y > diag.z ? 1 : 2);
    auto cmpShapeFunc = [maxAxis](const T &s1, const T &s2) -> bool {
        if (!s1.getBounds().isValid()) {
            return false;
        }
        return s1.getBounds().center()[maxAxis] < s2.getBounds().center()[maxAxis];
    };
    std::array<std::vector<T>, 2> childShapes = split_shapes<T>(std::move(node->shapes), cmpShapeFunc, 0.5f);
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
