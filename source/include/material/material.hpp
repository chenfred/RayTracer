#pragma once

#include <glm/glm.hpp>

class Material {
public:
    Material() = default;
    Material(const glm::vec3 &_emissive) : emitable{true}, emissive{_emissive} {}
    virtual ~Material() = default;

    virtual glm::vec3 sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo, glm::vec3 &beta) const = 0;
    bool isEmitable() const { return emitable; }
    glm::vec3 getEmissive() const { return emissive; }

    void setEmissive(const glm::vec3 &emi) {
        emitable = true;
        emissive = emi;
    }

private:
    bool emitable{false};
    glm::vec3 emissive{0};
};
