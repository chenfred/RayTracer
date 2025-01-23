#pragma once

#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "func/debug_helpers.hpp"
#include "func/graphics.hpp"
#include "shape/shape.hpp"
#include "util/concepts.hpp"

#include <array>
#include <cassert>
#include <optional>
#include <vector>

// TODO: 实现更高效的BVH（例如把递归转成迭代）
// FIXME: 修复导致dragon_871k.obj的三角形稀碎的问题

template <ShapeType T>
class BVH : public Shape {
public:
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bounds getBounds() const override;

    void build(std::vector<T> &&shapes);

private:
    // for flat structure
    struct Node {
        Bounds bounds;
        std::vector<T> shapes;
        size_t rchild;
    };
    std::vector<Node> nodes;
    std::optional<HitInfo> iterativeIntersect(const Ray &ray, float t_min, float t_max) const;

    // for tree structure
    struct TreeNode {
        Bounds bounds;
        std::vector<T> shapes;
        std::array<TreeNode *, 2> children;
        void updateBounds();
    };
    TreeNode *root;
    void recursiveIntersect(TreeNode *node, const Ray &ray, float t_min, float t_max, std::optional<HitInfo> &closestHit) const;
    void buildTree(std::vector<T> &&shapes);
    void recursiveSplit(TreeNode *node);
    size_t recursiveFlattern(TreeNode *node);
};

template <ShapeType T>
std::optional<HitInfo> BVH<T>::intersect(const Ray &ray, float t_min, float t_max) const {
    // TEST:
    // return iterativeIntersect(ray, t_min, t_max);

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
    // build treenode
    buildTree(std::move(shapes));
    recursiveFlattern(root);
}

template <ShapeType T>
std::optional<HitInfo> BVH<T>::iterativeIntersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closestHit;

    // 用栈来模拟递归调用
    std::vector<size_t> stack;
    constexpr size_t STACK_PREALLOC_SIZE = 16;
    stack.reserve(STACK_PREALLOC_SIZE);

    stack.push_back(0);
    while (!stack.empty()) {
        auto currNodeIndex = stack.back();

        const Node &node = nodes[currNodeIndex];

        if (!node.bounds.hasIntersection(ray, t_min, t_max)) {
            stack.pop_back(); // 模拟返回
        } else if (node.shapes.empty()) {
            // (递归左子节点，递归右子节点，返回）逆着来
            stack.pop_back();
            stack.push_back(node.rchild);
            stack.push_back(currNodeIndex + 1);
        } else {
            for (const auto &shape : node.shapes) {
                std::optional<HitInfo> hit = shape.intersect(ray, t_min, t_max);
                if (hit) {
                    t_max = hit->t;
                    closestHit = hit;
                }
            }
            stack.pop_back(); // 模拟返回
        }
    }

    debug_print(std::format("BVH::Intersect: Stack expand to {}", stack.capacity()), stack.capacity() > STACK_PREALLOC_SIZE);

    return closestHit;
}

template <ShapeType T>
void BVH<T>::TreeNode::updateBounds() {
    bounds = {};
    for (const T &shape : shapes) {
        bounds.expand(shape.getBounds());
    }
}

template <ShapeType T>
void BVH<T>::buildTree(std::vector<T> &&shapes) {
    root = new TreeNode{};
    root->shapes = std::move(shapes);
    root->updateBounds();
    recursiveSplit(root);
}

template <ShapeType T>
void BVH<T>::recursiveIntersect(TreeNode *treeNode, const Ray &ray, float t_min, float t_max, std::optional<HitInfo> &closestHit) const {
    if (!treeNode || !treeNode->bounds.hasIntersection(ray, t_min, t_max)) {
        return;
    }

    if (treeNode->shapes.empty()) {
        // non-leafnode
        for (auto child : treeNode->children) {
            std::optional<HitInfo> hit;
            recursiveIntersect(child, ray, t_min, t_max, hit);
            if (hit) {
                t_max = hit->t;
                closestHit = hit;
            }
        }
        return;
    }

    for (const auto &shape : treeNode->shapes) {
        std::optional<HitInfo> hit = shape.intersect(ray, t_min, t_max);
        if (hit) {
            t_max = hit->t;
            closestHit = hit;
        }
    }
}

template <ShapeType T>
void BVH<T>::recursiveSplit(TreeNode *treeNode) {
    if (treeNode->shapes.size() <= 1) {
        return;
    }

    const auto &bounds = treeNode->bounds;
    auto diag = bounds.diagonal();
    size_t maxAxis = diag.x > diag.y ? (diag.x > diag.z ? 0 : 2) : (diag.y > diag.z ? 1 : 2);
    auto cmpShapeFunc = [maxAxis](const T &s1, const T &s2) -> bool {
        if (!s1.getBounds().isValid()) {
            return false;
        }
        return s1.getBounds().center()[maxAxis] < s2.getBounds().center()[maxAxis];
    };
    std::array<std::vector<T>, 2> childShapes = split_shapes<T>(std::move(treeNode->shapes), cmpShapeFunc, 0.5f);
    treeNode->shapes.clear();
    treeNode->shapes.shrink_to_fit();

    bool stopRecursion = false;
    if (childShapes[0].empty() || childShapes[1].empty()) {
        stopRecursion = true;
    }
    for (size_t i = 0; i < 2; ++i) {
        if (childShapes[i].empty()) {
            continue;
        }

        TreeNode *child = new TreeNode{};
        treeNode->children[i] = child;
        child->shapes = childShapes[i];
        child->updateBounds();
        if (!stopRecursion) {
            recursiveSplit(child);
        }
    }
}

template <ShapeType T>
size_t BVH<T>::recursiveFlattern(TreeNode *treeNode) {
    size_t currIndex = nodes.size();

    nodes.emplace_back(treeNode->bounds, std::vector(treeNode->shapes), 0);
    if (treeNode->shapes.empty()) { // 非叶节点
        recursiveFlattern(treeNode->children[0]);
        nodes[currIndex].rchild = recursiveFlattern(treeNode->children[1]);
    }

    return currIndex;
}