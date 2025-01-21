#pragma once

#include "material/material.hpp"

class DiffuseMaterial : public Material {
public:
    DiffuseMaterial(const glm::vec3 &_albedo) : DiffuseMaterial(glm::vec3{0}, _albedo) {}
    DiffuseMaterial(const glm::vec3 &_emissive, const glm::vec3 &_albedo) : Material{_emissive}, albedo{_albedo} {}

    ScatteringType getScatteringType() const override { return ScatteringType::DIFFUSE; }
    glm::vec3 sampleDirectionLocalized(const glm::vec3 &wi, const RNG& rng) const override;
    glm::vec3 sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo) const override;

private:
    glm::vec3 albedo;
};

class SpecularMaterial : public Material {
public:
    SpecularMaterial(const glm::vec3 &_reflect) : SpecularMaterial(glm::vec3{0}, _reflect) {}
    SpecularMaterial(const glm::vec3 &_emissive, const glm::vec3 &_reflect) : Material{_emissive}, reflectance{_reflect} {}

    ScatteringType getScatteringType() const override { return ScatteringType::SPECULAR; }
    glm::vec3 sampleDirectionLocalized(const glm::vec3 &wi, const RNG& rng) const override;
    glm::vec3 sampleBSDF(const glm::vec3 &wi, const glm::vec3 &wo) const override;

private:
    glm::vec3 reflectance;
};