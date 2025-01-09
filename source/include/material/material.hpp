#pragma once

#include <glm/glm.hpp>

class Material {
public:
    Material() = default;
    Material(const glm::vec3 &_emissive) : emissive{_emissive} {}
    virtual ~Material() = default;

    virtual glm::vec3 sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo, glm::vec3 &beta) const = 0;
    void setEmissive(const glm::vec3 &emi) { emissive = emi; }

private:
    glm::vec3 emissive{};
};