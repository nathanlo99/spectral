
#pragma once

#include "material.hpp"

struct ReflectiveMaterial : Material {
  vec3 albedo;
  real fuzz;

  ReflectiveMaterial(const vec3 &albedo, const real fuzz)
      : albedo(albedo), fuzz(fuzz) {}
  virtual ~ReflectiveMaterial() {}

  virtual bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
                       vec3 &attenuation, Ray &scattered) const override;
};
