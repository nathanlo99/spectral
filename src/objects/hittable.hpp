
#pragma once

#include "util/ray.hpp"
#include "util/util.hpp"

struct HitRecord {
  vec3 p;
  vec3 normal;
  real t;
  bool front_face;

  constexpr inline void set_face_normal(const Ray &ray,
                                        const vec3 &outward_normal) {
    front_face = glm::dot(ray.direction, outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

struct Hittable {
public:
  virtual bool hit(const Ray &r, real t_min, real t_max,
                   HitRecord &rec) const = 0;
};
