
#pragma once

#include "material.hpp"

struct DiffuseMaterial : public Material {
  vec3 albedo;

  DiffuseMaterial(const vec3 &albedo) : albedo(albedo) {}
  virtual ~DiffuseMaterial() {}

  virtual bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
                       vec3 &attenuation, Ray &scattered) const override;
};
