
#pragma once

#include "material.hpp"

class DiffuseMaterial : public Material {
public:
  DiffuseMaterial(const vec3 &albedo) : albedo(albedo) {}
  virtual ~DiffuseMaterial() {}

  virtual bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
                       vec3 &attenuation, Ray &scattered) const override;

public:
  vec3 albedo;
};
