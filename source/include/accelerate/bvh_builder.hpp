#pragma once

#include "accelerate/bounds.hpp"
#include "func/debug_helpers.hpp"
#include "func/graphics.hpp"
#include "util/concepts.hpp"

struct alignas(32) BVHNode {
    Bounds bounds;
    uint32_t shapesSize;
    union { // 减少占用，用于对齐32字节
        uint32_t shapesBegin;
        uint32_t rchild;
    };
};

template <ShapeType T>
class BVHBuilder {
private:
    struct BVHBuildTreeNode {
        Bounds bounds;
        std::vector<T> shapes;
        std::array<BVHBuildTreeNode *, 2> children;
        void updateBounds() {
            bounds = {};
            for (const T &shape : shapes) {
                bounds.expand(shape.getBounds());
            }
        }
    };
    struct BVHBuildState {
        uint32_t numTotalNodes{0}, numLeafNodes{0}, numTotalShapes{0}, maxLeafShapesNum{0};
        void addLeafNode(BVHBuildTreeNode *treeNode) {
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

public:
    BVHBuilder(std::vector<T> &_shapes) : shapes{_shapes} {}
    std::vector<BVHNode> build();

private:
    std::vector<T> &shapes;
    std::vector<BVHNode> nodes;

    void recursiveSplit(BVHBuildTreeNode *treeNode, BVHBuildState &state);
    uint32_t recursiveFlattern(BVHBuildTreeNode *treeNode, BVHBuildState &state);
};

template <ShapeType T>
std::vector<BVHNode> BVHBuilder<T>::build() {
    // 初始化状态
    nodes.clear();
    nodes.shrink_to_fit();

    // 构造树形结构
    BVHBuildTreeNode *root = new BVHBuildTreeNode{};
    root->shapes = std::move(shapes);
    root->updateBounds();
    DEBUG_PRINT(std::format("Building BVH with {} shape(s).", root->shapes.size()))
    BVHBuildState buildTreeState{};
    recursiveSplit(root, buildTreeState);
    DEBUG_LINE(buildTreeState.print());

    // 转化为平坦列表
    nodes.clear();
    nodes.shrink_to_fit();
    DEBUG_PRINT(std::format("Flatterning BVH with {} shape(s).", shapes.size()))
    BVHBuildState flatternState{};
    recursiveFlattern(root, flatternState);
    DEBUG_LINE(flatternState.print());

    return nodes;
}

template <ShapeType T>
void BVHBuilder<T>::recursiveSplit(BVHBuildTreeNode *treeNode, BVHBuildState &state) {
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

        BVHBuildTreeNode *child = new BVHBuildTreeNode{};
        treeNode->children[i] = child;
        child->shapes = childShapes[i];
        child->updateBounds();
        if (!stopRecursion) {
            recursiveSplit(child, state);
        }
    }
}

template <ShapeType T>
uint32_t BVHBuilder<T>::recursiveFlattern(BVHBuildTreeNode *treeNode, BVHBuildState &state) {
    state.numTotalNodes++;

    uint32_t currIndex = nodes.size();
    nodes.emplace_back(BVHNode{.bounds = treeNode->bounds, .shapesSize = 0, .shapesBegin = 0});

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