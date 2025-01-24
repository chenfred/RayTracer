#pragma once

#include "accelerate/bounds.hpp"
#include "camera/ray.hpp"
#include "func/debug_helpers.hpp"
#include "func/graphics.hpp"
#include "shape/shape.hpp"
#include "util/concepts.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <format>
#include <optional>
#include <vector>

template <ShapeType T>
class BVH : public Shape {
public:
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
    Bounds getBounds() const override;

    void build(std::vector<T> &&shapes);

private:
    std::vector<T> shapes;
    struct alignas(32) Node {
        Bounds bounds;
        uint32_t shapesSize;
        union { // 减少占用，用于对齐32字节
            uint32_t shapesBegin;
            uint32_t rchild;
        };
    };
    std::vector<Node> nodes;

    struct TreeNode {
        Bounds bounds;
        std::vector<T> shapes;
        std::array<TreeNode *, 2> children;
        void updateBounds();
    };

    struct BVHBuildState {
        uint32_t numTotalNodes{0}, numLeafNodes{0}, numTotalShapes{0}, maxLeafShapesNum{0};
        void addLeafNode(TreeNode *treeNode) {
            numLeafNodes++;
            numTotalShapes += treeNode->shapes.size();
            maxLeafShapesNum = std::max<uint32_t>(maxLeafShapesNum, treeNode->shapes.size());
        }
        void print() const {
            std::cout << std::format("NumTotalNodes: {}", numTotalNodes) << std::endl;
            std::cout << std::format("numLeafNodes: {}", numLeafNodes) << std::endl;
            std::cout << std::format("numTotalShapes: {}", numTotalShapes) << std::endl;
            std::cout << std::format("maxLeafShapesNum: {}", maxLeafShapesNum) << std::endl;
        }
    };

    void buildTree(std::vector<T> &&shapes, TreeNode *&root, BVHBuildState &state);
    void recursiveSplit(TreeNode *node, BVHBuildState &state);
    uint32_t recursiveFlattern(TreeNode *node, BVHBuildState &state);
};

template <ShapeType T>
std::optional<HitInfo> BVH<T>::intersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closestHit;

    // 用栈来模拟递归调用
    std::vector<uint32_t> stack;
    constexpr uint32_t STACK_PREALLOC_SIZE = 32;
    stack.reserve(STACK_PREALLOC_SIZE);

    stack.push_back(0);
    while (!stack.empty()) {
        auto currNodeIndex = stack.back();

        const Node &node = nodes[currNodeIndex];
        if (!node.bounds.hasIntersection(ray, t_min, t_max)) {
            stack.pop_back(); // 模拟返回
        } else if (node.shapesSize == 0) {
            // (递归左子节点，递归右子节点，返回）逆着来
            stack.pop_back();
            stack.push_back(node.rchild);
            stack.push_back(currNodeIndex + 1);
        } else {
            auto nodeShapesEnd = node.shapesBegin + node.shapesSize;
            for (auto i = node.shapesBegin; i < nodeShapesEnd; ++i) {
                const Shape &shape = shapes[i];
                std::optional<HitInfo> hit = shape.intersect(ray, t_min, t_max);
                if (hit) {
                    t_max = hit->t;
                    closestHit = hit;
                }
            }
            stack.pop_back(); // 模拟返回
        }
    }

    DEBUG_PRINT(std::format("BVH::Intersect: Stack expand to {}", stack.capacity()), stack.capacity() > STACK_PREALLOC_SIZE)

    return closestHit;
}

template <ShapeType T>
Bounds BVH<T>::getBounds() const {
    if (nodes.empty()) {
        return {};
    }
    return nodes.front().bounds;
}

template <ShapeType T>
void BVH<T>::build(std::vector<T> &&shapes) {
    DEBUG_PRINT(std::format("Building BVH with {} shape(s).", shapes.size()))
    BVHBuildState buildTreeState{};
    // build tree structure
    TreeNode *root{};
    buildTree(std::move(shapes), root, buildTreeState);
    DEBUG_LINE(buildTreeState.print());
    // convert to linear structure
    DEBUG_PRINT(std::format("Flatterning BVH with {} shape(s).", shapes.size()))
    BVHBuildState flatternState{};
    recursiveFlattern(root, flatternState);
    DEBUG_LINE(flatternState.print());
}

template <ShapeType T>
void BVH<T>::TreeNode::updateBounds() {
    bounds = {};
    for (const T &shape : shapes) {
        bounds.expand(shape.getBounds());
    }
}

template <ShapeType T>
void BVH<T>::buildTree(std::vector<T> &&shapes, TreeNode *&root, BVHBuildState &state) {
    root = new TreeNode{};
    root->shapes = std::move(shapes);
    root->updateBounds();
    recursiveSplit(root, state);
}

template <ShapeType T>
void BVH<T>::recursiveSplit(TreeNode *treeNode, BVHBuildState &state) {
    state.numTotalNodes++;
    if (treeNode->shapes.size() <= 1) {
        DEBUG_ASSERT(treeNode->shapes.size() == 1)
        state.addLeafNode(treeNode);
        return;
    }

    const auto &bounds = treeNode->bounds;
    auto diag = bounds.diagonal();
    auto maxAxis = diag.x > diag.y ? (diag.x > diag.z ? 0 : 2) : (diag.y > diag.z ? 1 : 2);
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
    for (auto i = 0; i < 2; ++i) {
        if (childShapes[i].empty()) {
            continue;
        }

        TreeNode *child = new TreeNode{};
        treeNode->children[i] = child;
        child->shapes = childShapes[i];
        child->updateBounds();
        if (!stopRecursion) {
            recursiveSplit(child, state);
        }
    }
}

template <ShapeType T>
uint32_t BVH<T>::recursiveFlattern(TreeNode *treeNode, BVHBuildState &state) {
    state.numTotalNodes++;

    uint32_t currIndex = nodes.size();
    nodes.emplace_back(Node{.bounds = treeNode->bounds, .shapesSize = 0, .shapesBegin = 0});

    if (treeNode->shapes.empty()) { // 非叶节点
        recursiveFlattern(treeNode->children[0], state);
        nodes[currIndex].rchild = recursiveFlattern(treeNode->children[1], state);
    } else {
        state.addLeafNode(treeNode);
        nodes[currIndex].shapesSize = treeNode->shapes.size();
        nodes[currIndex].shapesBegin = shapes.size();
        shapes.insert(shapes.end(), treeNode->shapes.begin(), treeNode->shapes.end());
    }
    delete treeNode;

    return currIndex;
}

// template <ShapeType T>
// void BVH<T>::recursiveIntersect(TreeNode *treeNode, const Ray &ray, float t_min, float t_max, std::optional<HitInfo> &closestHit) const {
//     if (!treeNode || !treeNode->bounds.hasIntersection(ray, t_min, t_max)) {
//         return;
//     }

//     if (treeNode->shapes.empty()) {
//         // non-leafnode
//         for (auto child : treeNode->children) {
//             std::optional<HitInfo> hit;
//             recursiveIntersect(child, ray, t_min, t_max, hit);
//             if (hit) {
//                 t_max = hit->t;
//                 closestHit = hit;
//             }
//         }
//         return;
//     }

//     for (const auto &shape : treeNode->shapes) {
//         std::optional<HitInfo> hit = shape.intersect(ray, t_min, t_max);
//         if (hit) {
//             t_max = hit->t;
//             closestHit = hit;
//         }
//     }
// }