#include "camera/frame.hpp"
#include "util/globals.hpp"

// TODO: 推一下这个坐标空间的变换
// FIXME: LocalFrame的ys不是normal方向？怎么解决
LocalFrame::LocalFrame(const glm::vec3 &normal) {
    ys = normal;
    glm::vec3 up = glm::abs(normal.y) < 1.0f - FLOAT_CMP_EPS ? glm::vec3(0, 1, 0) : glm::vec3(0, 0, 1);
    xs = glm::normalize(glm::cross(up, normal));
    zs = glm::normalize(glm::cross(xs, ys));
}

glm::vec3 LocalFrame::toLocal(const glm::vec3 &worldDirection) const {
    // 坐标就是在xs,ys,zs上的投影
    return glm::normalize(glm::vec3{
        glm::dot(worldDirection, xs),
        glm::dot(worldDirection, ys),
        glm::dot(worldDirection, zs)});
}

glm::vec3 LocalFrame::toWorld(const glm::vec3 &localDirection) const {
    return glm::normalize(localDirection.x * xs + localDirection.y * ys + localDirection.z * zs);
}
