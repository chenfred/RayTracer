#pragma once

#include "accelerate/bounds.hpp"
#include "func/debug_helpers.hpp"
#include "func/graphics.hpp"
#include "util/concepts.hpp"

#include <cstdint>

constexpr uint8_t BVH_INTERSECT_STACK_PREALLOC_SIZE = 32;

struct alignas(32) BVHNode {
    Bounds bounds;
    uint8_t axis;
    uint8_t depth;
    uint16_t numShapes;
    union { // 减少占用，用于对齐32字节
        uint32_t indexShapesBegin;
        uint32_t indexRChild;
    };
};

template <HasGetBounds T>
struct BVHBuildTreeNode {
    Bounds bounds;
    std::vector<T> shapes;
    std::array<BVHBuildTreeNode *, 2> children;
    uint8_t axis;
    uint8_t depth;
    void updateBounds() {
        bounds = {};
        for (const T &shape : shapes) {
            bounds.expand(shape.getBounds());
        }
    }
};

template <HasGetBounds T>
struct BVHBuildState {
    uint32_t numTotalNodes{0}, numLeafNodes{0}, numTotalShapes{0}, maxLeafShapesNum{0}, maxDepth{0};
    void addLeafNode(BVHBuildTreeNode<T> *treeNode) {
        numLeafNodes++;
        numTotalShapes += treeNode->shapes.size();
        maxLeafShapesNum = std::max<decltype(maxLeafShapesNum)>(maxLeafShapesNum, treeNode->shapes.size());
        maxDepth = std::max<decltype(maxDepth)>(maxDepth, treeNode->depth);
    }
    void print() const {
        std::cout << std::format(" - NumTotalNodes: {}", numTotalNodes) << std::endl;
        std::cout << std::format(" - numLeafNodes: {}", numLeafNodes) << std::endl;
        std::cout << std::format(" - numTotalShapes: {}", numTotalShapes) << std::endl;
        std::cout << std::format(" - maxLeafShapesNum: {}", maxLeafShapesNum) << std::endl;
        std::cout << std::format(" - maxDepth: {}", maxDepth) << std::endl;
    }
};

template <HasGetBounds T>
class BVHBuildTreeNodeAllocator {
public:
    BVHBuildTreeNodeAllocator() : nextNodePtr(CHUNK_SIZE) {}
    ~BVHBuildTreeNodeAllocator() {
        for (auto *chunk : nodeChunks) {
            delete[] chunk;
        }
        nodeChunks.clear();
    }

    BVHBuildTreeNode<T> *allocateNode() {
        if (nextNodePtr == CHUNK_SIZE) {
            nodeChunks.push_back(new BVHBuildTreeNode<T>[CHUNK_SIZE]);
            nextNodePtr = 0;
        }
        return &(nodeChunks.back()[nextNodePtr++]);
    }

private:
    std::vector<BVHBuildTreeNode<T> *> nodeChunks;
    size_t nextNodePtr;
    static constexpr size_t CHUNK_SIZE = 4096;
};

template <HasGetBounds T>
class BVHNodesBuilder {
private:
public:
    static std::vector<BVHNode> BuildBVHNodes(std::vector<T> &shapes) { return BVHNodesBuilder(shapes).build(); }

private:
    std::vector<T> &shapes;
    std::vector<BVHNode> nodes{};
    BVHBuildTreeNodeAllocator<T> treeNodeAllocator{};

    BVHNodesBuilder(std::vector<T> &_shapes) : shapes{_shapes} {} // 隐藏构造器
    std::vector<BVHNode> build();
    void recursiveSplit(BVHBuildTreeNode<T> *treeNode, BVHBuildState<T> &state);
    uint32_t recursiveFlattern(BVHBuildTreeNode<T> *treeNode);
};

template <HasGetBounds T>
std::vector<BVHNode> BVHNodesBuilder<T>::build() {
    // 初始化状态
    nodes.clear();

    // 构造树形结构
    BVHBuildTreeNode<T> *root = treeNodeAllocator.allocateNode();
    root->shapes = std::move(shapes);
    root->updateBounds();
    root->depth = 0;
    DEBUG_PRINT(std::format("Building BVH with {} shape(s):", root->shapes.size()))
    BVHBuildState<T> buildTreeState{};
    recursiveSplit(root, buildTreeState);
    DEBUG_LINE(buildTreeState.print());

    // 转化为平坦列表
    nodes.reserve(buildTreeState.numTotalNodes);
    shapes.reserve(buildTreeState.numTotalShapes);
    recursiveFlattern(root);

    return nodes;
}

template <HasGetBounds T>
void BVHNodesBuilder<T>::recursiveSplit(BVHBuildTreeNode<T> *treeNode, BVHBuildState<T> &state) {
    state.numTotalNodes++;
    if (treeNode->shapes.size() <= 1 || treeNode->depth >= BVH_INTERSECT_STACK_PREALLOC_SIZE - 1) {
        state.addLeafNode(treeNode);
        return;
    }
    // 传统BVH节点划分 =============================================
    const auto f_splitShapes_conventional = [&](size_t &retSplitAxis, size_t &retSplitPos) -> void {
        auto diag = treeNode->bounds.diagonal();
        uint8_t maxAxis = diag.x > diag.y ? (diag.x > diag.z ? 0 : 2) : (diag.y > diag.z ? 1 : 2);
        retSplitAxis = maxAxis;
        retSplitPos = treeNode->shapes.size() / 2;
    };
    // SAH优化分割 =================================================
    const auto f_splitShapes_SAH = [&](size_t &retSplitAxis, size_t &retSplitPos) -> void {
        // SAH优化分割策略 - 多轴评估版本
        float globalMinCost = std::numeric_limits<float>::max();
        size_t bestSplitPos = 0;
        uint8_t bestAxis = 0;
        const float parentArea = treeNode->bounds.area();

        // 遍历三个轴
        for (uint8_t axis = 0; axis < 3; ++axis) {
            // 按当前轴排序
            std::sort(treeNode->shapes.begin(), treeNode->shapes.end(),
                      [axis](const T &a, const T &b) {
                          return a.getBounds().center()[axis] < b.getBounds().center()[axis];
                      });

            // 预计算包围盒
            std::vector<Bounds> leftBounds(treeNode->shapes.size());
            Bounds currentLeft;
            for (size_t i = 0; i < treeNode->shapes.size(); ++i) {
                currentLeft.expand(treeNode->shapes[i].getBounds());
                leftBounds[i] = currentLeft;
            }

            std::vector<Bounds> rightBounds(treeNode->shapes.size());
            Bounds currentRight;
            for (int i = treeNode->shapes.size() - 1; i >= 0; --i) {
                currentRight.expand(treeNode->shapes[i].getBounds());
                rightBounds[i] = currentRight;
            }

            // 采样策略：当形状数量超过8时只采样8个点
            const size_t sampleCount = treeNode->shapes.size() > 8 ? 8 : treeNode->shapes.size();
            float localMinCost = std::numeric_limits<float>::max();
            size_t localSplitPos = 0;

            for (size_t s = 0; s < sampleCount; ++s) {
                size_t i = (s * (treeNode->shapes.size() - 1)) / (sampleCount - 1); // 均匀采样

                if (i == 0 || i >= treeNode->shapes.size())
                    continue;

                const float leftArea = leftBounds[i - 1].area();
                const float rightArea = rightBounds[i].area();
                const float cost = (leftArea * i + rightArea * (treeNode->shapes.size() - i)) / parentArea;

                DEBUG_ASSERT(leftArea > 0 && rightArea > 0);
                DEBUG_ASSERT(leftArea <= parentArea && rightArea <= parentArea);

                if (cost < localMinCost) {
                    localMinCost = cost;
                    localSplitPos = i;
                }
            }

            DEBUG_ASSERT(localSplitPos > 0 && localSplitPos < treeNode->shapes.size());

            if (localMinCost < globalMinCost) {
                globalMinCost = localMinCost;
                bestSplitPos = localSplitPos;
                bestAxis = axis;
            }
        }

        retSplitAxis = bestAxis;
        retSplitPos = bestSplitPos;
    }; // SAH的子节点划分法，备用，别给删了
    // ============================================================

    size_t splitAxis, spiltPos;
    f_splitShapes_conventional(splitAxis, spiltPos);
    treeNode->axis = splitAxis;
    const auto f_splitCompareShapes = [splitAxis](const T &s1, const T &s2) -> bool { return s1.getBounds().center()[splitAxis] < s2.getBounds().center()[splitAxis]; };
    std::array<std::vector<T>, 2> childShapes = split_shapes<T>(std::move(treeNode->shapes), f_splitCompareShapes, spiltPos);
    treeNode->shapes.clear();
    treeNode->shapes.shrink_to_fit();

    // TODO: 这里逻辑比较混乱，优化一下
    bool stopRecursion = false;
    if (childShapes[0].empty() || childShapes[1].empty()) {
        stopRecursion = true;
        if (childShapes[0].empty() && childShapes[1].empty()) {
            state.addLeafNode(treeNode);
            return;
        }
    }
    for (auto i = 0; i < 2; ++i) {
        if (childShapes[i].empty()) {
            continue;
        }

        BVHBuildTreeNode<T> *child = treeNodeAllocator.allocateNode();
        treeNode->children[i] = child;
        child->shapes = childShapes[i];
        child->updateBounds();
        child->depth = treeNode->depth + 1;
        if (!stopRecursion) {
            recursiveSplit(child, state);
        }
    }
}

template <HasGetBounds T>
uint32_t BVHNodesBuilder<T>::recursiveFlattern(BVHBuildTreeNode<T> *treeNode) {

    uint32_t currIndex = nodes.size();
    nodes.emplace_back(BVHNode{treeNode->bounds, treeNode->axis, treeNode->depth, 0, 0});

    if (treeNode->shapes.empty()) { // 非叶节点
        recursiveFlattern(treeNode->children[0]);
        nodes[currIndex].indexRChild = recursiveFlattern(treeNode->children[1]);
    } else {
        nodes[currIndex].numShapes = treeNode->shapes.size();
        nodes[currIndex].indexShapesBegin = shapes.size();
        shapes.insert(shapes.end(), treeNode->shapes.begin(), treeNode->shapes.end());
    }

    return currIndex;
}
