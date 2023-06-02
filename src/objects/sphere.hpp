
#pragma once

#include "hittable.hpp"

#include "util/util.hpp"

struct HitRecord;
struct Material;

struct Sphere : Hittable {
  vec3 center = vec3(0.0);
  real radius = 1.0;
  std::shared_ptr<Material> material;

  Sphere(const vec3 &center, const real radius,
         std::shared_ptr<Material> material)
      : center(center), radius(radius), material(material) {}
  virtual ~Sphere() {}

  virtual bool hit(const Ray &ray, real t_min, real t_max,
                   HitRecord &record) const override;
};
