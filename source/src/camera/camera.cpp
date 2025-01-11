#include "camera/camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// TODO: 修复并理解相机的坐标系和变换（目前是默认的右手系版本）
Camera::Camera(Film &_film, const glm::vec3 &_pos, const glm::vec3 &direction, float yAngleFOV) : film{_film}, position{_pos} {
    clip2cameraMat = glm::inverse(glm::perspective(glm::radians(yAngleFOV), film.getAspectRatio(), 1.0f, 2.0f));
    camera2worldMat = glm::inverse(glm::lookAt(position, position + glm::normalize(direction), {0, 1, 0}));
}

Ray Camera::generateEyeRay(const glm::ivec2 &pixel_coord, const glm::vec2 &in_pixel_offset) const {
    glm::vec2 ndc_xy = (glm::vec2(pixel_coord) + in_pixel_offset) / glm::vec2{film.getWidth(), film.getHeight()};
    ndc_xy.y = 1.0f - ndc_xy.y;    // 屏幕空间左上角为原点，先转成左下角为原点
    ndc_xy = ndc_xy * 2.0f - 1.0f; //[0,1]->[0,2]->[-1,1]

    glm::vec4 clip_coord{ndc_xy, -1.0f, 1.0f}; // TODO: 理解这个转换的意义（w分量与zNear有关？），推测z应该表示NDC空间里近平面的位置
    glm::vec3 world_pos{camera2worldMat * clip2cameraMat * clip_coord};

    return Ray(position, glm::normalize(world_pos - position));
}

// Ray Camera::generateEyeRay(const glm::ivec2 &pixel_coord, const glm::vec2 &in_pixel_offset) const {
//     glm::vec2 ndc_xy = (glm::vec2(pixel_coord) + in_pixel_offset) / glm::vec2{film.getWidth(), film.getHeight()};
//     ndc_xy.y = 1.0f - ndc_xy.y;    // 屏幕空间左上角为原点，先转成左下角为原点
//     ndc_xy = ndc_xy * 2.0f - 1.0f; //[0,1]->[0,2]->[-1,1]

//     glm::vec4 clip_coord{ndc_xy, 0.0f, 1.0f}; // TODO: 理解这个转换的意义（w分量与zNear有关？），推测z应该表示NDC空间里近平面的位置
//     glm::vec3 world_pos{camera2worldMat * clip2cameraMat * clip_coord};

//     return Ray(position, glm::normalize(world_pos - position));
// }


