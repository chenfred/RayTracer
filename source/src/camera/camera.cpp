#include "camera/camera.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "glm/trigonometric.hpp"

Camera::Camera(Film &_film, const glm::vec3 &_pos, const glm::vec3 &lookAtPoint, float fovY) : film{_film}, position{_pos} {
    clip2cameraMat = glm::inverse(glm::perspective(glm::radians(fovY), film.getAspectRatio(), 1.0f, 2.0f));
    camera2worldMat = glm::inverse(glm::lookAt(position, lookAtPoint, {0, 1, 0}));
}

Ray Camera::generateEyeRay(const glm::ivec2 &pixel_coord, const glm::vec2 &in_pixel_offset) const {
    glm::vec2 ndc_xy = (glm::vec2(pixel_coord) + in_pixel_offset) / glm::vec2{film.getWidth(), film.getHeight()};
    ndc_xy.y = 1.0f - ndc_xy.y;    // 屏幕空间左上角为原点，先转成左下角为原点
    ndc_xy = ndc_xy * 2.0f - 1.0f; //[0,1]->[0,2]->[-1,1]

    glm::vec4 clip_coord{ndc_xy, 0.0f, 1.0f}; // TODO: 理解这个转换的意义（与zNear有关）
    glm::vec3 world_pos{camera2worldMat * clip2cameraMat * clip_coord};

    return Ray(position, glm::normalize(world_pos - position));
}