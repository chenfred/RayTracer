#pragma once

#include <glm/glm.hpp>

// 局部坐标空间，用于方便算光线弹射和采样半球
class LocalFrame {
public:
    // 以传入的normal为y正轴构建局部坐标系
    LocalFrame(const glm::vec3 &normal);

    glm::vec3 toLocal(const glm::vec3 &worldDirection) const;
    glm::vec3 toWorld(const glm::vec3 &localDirection) const;

private:
    glm::vec3 xs, ys, zs; // 三轴坐标
};