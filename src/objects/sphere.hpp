
#pragma once

#include "hittable.hpp"

#include "objects/bounding_box.hpp"
#include "util/util.hpp"

struct HitRecord;
struct Material;

struct Sphere : public Hittable {
  const vec3 center = vec3(0.0);
  const real radius = 1.0;
  const std::shared_ptr<Material> material;

  Sphere(const vec3 &center, const real radius,
         std::shared_ptr<Material> material)
      : center(center), radius(radius), material(material) {
    debug_assert(radius > 0.0, "Sphere radius must be positive.");
  }
  virtual ~Sphere() {}

  constexpr static vec2 get_uv(const vec3 &point);

  virtual bool hit(const Ray &ray, const real t_min, const real t_max,
                   HitRecord &record) const override;
  virtual BoundingBox bounding_box() const override;
};
