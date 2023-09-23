
#pragma once

#include "material.hpp"

struct DielectricMaterial : public Material {
  const vec3 albedo;
  const real refractive_index;

  constexpr DielectricMaterial(const vec3 &albedo, const real refractive_index)
      : albedo(albedo), refractive_index(refractive_index) {}
  virtual ~DielectricMaterial() {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               vec3 &attenuation, Ray &scattered) const override;
};
