#pragma once

#include "material/material.hpp"

class DiffuseMaterial : public Material {
public:
    DiffuseMaterial(const glm::vec3 &_albedo) : DiffuseMaterial({}, _albedo) {}
    DiffuseMaterial(const glm::vec3 &_emissive, const glm::vec3 &_albedo) : Material{_emissive}, albedo{_albedo} {}

    glm::vec3 sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo, glm::vec3 &beta) const override;

private:
    glm::vec3 albedo;
};