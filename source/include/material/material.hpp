#pragma once

#include "func/tools.hpp"
#include "util/rng.hpp"

#include <glm/glm.hpp>

enum class ScatteringType {
    NONE,
    DIFFUSE,
    SPECULAR,
    TRANSPARENT,
};

class Material {
public:
    Material() = default;
    Material(const glm::vec3 &_emissive) : emissive{_emissive} {}
    virtual ~Material() = default;

    virtual ScatteringType getScatteringType() const { return ScatteringType::NONE; }
    // 在以法线为Y+的局部坐标系采样wo方向，其中wi和wo均以散射点为起点
    virtual glm::vec3 sampleDirectionLocalized(const glm::vec3 &wi, const RNG &rng) const { return {}; };
    // 采样散射率，wi和wo均以散射点为起点，beta为光线弹射的累计衰减
    virtual glm::vec3 sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo) const { return {}; };
    bool isEmitable() const { return !vec_less_equal(emissive, glm::vec3{0}); }
    glm::vec3 getEmissive() const { return emissive; }

    void setEmissive(const glm::vec3 &emi) {
        emissive = emi;
    }

protected:
    glm::vec3 emissive{0};
};
