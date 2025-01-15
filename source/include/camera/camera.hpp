#pragma once

#include "film.hpp"
#include "ray.hpp"

class Camera {
public:
    Camera(Film &_film, const glm::vec3 &_pos, const glm::vec3 &direction, float yAngleFOV);
    ~Camera() = default;

    Ray generateEyeRay(const glm::ivec2 &pixel_coord, const glm::vec2 &in_pixel_offset = {0.5, 0.5}) const;

    Film &getFilm() { return film; }
    const Film &getFilm() const { return film; }

private:
    Film &film;
    glm::vec3 position;
    glm::mat4 clip2cameraMat, camera2worldMat;
};


