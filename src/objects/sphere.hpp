
#pragma once

#include "hittable.hpp"

#include "util/util.hpp"

struct Sphere : Hittable {
  vec3 center = vec3(0.0);
  real radius = 1.0;

  constexpr Sphere(const vec3 &center, const real radius)
      : center(center), radius(radius) {}
  virtual ~Sphere() {}

  virtual bool hit(const Ray &ray, real t_min, real t_max,
                   HitRecord &record) const override;
};
