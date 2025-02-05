#pragma once

#include "func/debug_helpers.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

inline void test_glm_lookAt(){
    glm::vec3 cameraPos = glm::vec3(0.0f, 6.0f, 1.0f);
    glm::vec3 lookAtPoint = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 viewMatrix = glm::lookAt(cameraPos, lookAtPoint, upVector);

    glm::vec4 point = glm::vec4(-1.0f, -1.0f, -5.0f, 1.0f);
    glm::vec4 transformedPoint = viewMatrix * point;
    print_vec(transformedPoint);
    transformedPoint/=transformedPoint.w;
    print_vec(transformedPoint);

}

inline void test_glm_persp(){
    // 创建透视投影矩阵
    float near = 0.1f, far = 100.0f;
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, near, far);
    print_mat(proj);

    // 定义观察空间中的点（右手系，Z轴指向屏幕外）
    glm::vec4 nearPoint(0.0f, 0.0f, -near, 1.0f);  // 近裁剪面
    glm::vec4 farPoint(0.0f, 0.0f, -far, 1.0f);    // 远裁剪面

    // 应用投影矩阵，转换到裁剪空间
    glm::vec4 projNear = proj * nearPoint;
    glm::vec4 projFar = proj * farPoint;
    print_vec(projNear);
    print_vec(projFar);

    // 透视除法（将齐次坐标转为标准设备坐标）
    projNear /= projNear.w;
    projFar /= projFar.w;

    std::cout << "Projected Near Point (NDC): (" 
              << projNear.x << ", " << projNear.y << ", " << projNear.z << ")\n";
    std::cout << "Projected Far Point (NDC): (" 
              << projFar.x << ", " << projFar.y << ", " << projFar.z << ")\n";

    // 检查 Z 值是否符合左手系（远裁剪面 Z > 近裁剪面 Z）
    if (projFar.z > projNear.z) {
        std::cout << "Z increases with depth: Left-handed coordinate system.\n";
    } else {
        std::cout << "Z decreases with depth: Right-handed coordinate system.\n";
    }
}