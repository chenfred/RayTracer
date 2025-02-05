#include "camera/camera.hpp"
#include "func/tools.hpp"
#include "util/globals.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(Film &_film, const glm::vec3 &_pos, const glm::vec3 &_lookAtPoint, float yAngleFOV) : film{_film}, position{_pos} {
    // glm::perspective会将右手系转为左手系（z轴会被反转，矩阵将坐标裁剪到[-1,1]^3，其中z值越大离近平面越远，zNear->-1、zFar->1）
    clip2viewMat = glm::inverse(glm::perspective(glm::radians(yAngleFOV), film.getAspectRatio(), 1.0f, 10.0f));
    // 由于glm::lookAt的实现，lookAtPoint.z-pos.z不能为0！！
    if (fp_equal(position.z, _lookAtPoint.z)) {
        position.z += FLOAT_CMP_EPS;
    }
    view2worldMat = glm::inverse(glm::lookAt(position, _lookAtPoint, {0, 1, 0}));
}

Ray Camera::generateEyeRay(const glm::ivec2 &pixel_coord, const glm::vec2 &in_pixel_offset) const {
    glm::vec2 ndc_xy = (glm::vec2(pixel_coord) + in_pixel_offset) / glm::vec2{film.getWidth(), film.getHeight()};
    ndc_xy.y = 1.0f - ndc_xy.y;    // 屏幕空间左上角为原点，先转成左下角为原点
    ndc_xy = ndc_xy * 2.0f - 1.0f; //[0,1]->[0,2]->[-1,1]

    glm::vec4 clip_coord{ndc_xy, -1.0f, 1.0f};
    glm::vec3 world_pos{view2worldMat * clip2viewMat * clip_coord};

    Ray eyeRay{position, glm::normalize(world_pos - position)};
    // eyeRay.print();
    return eyeRay;
}