
#pragma once

#include "util/ray.hpp"
#include "util/util.hpp"

struct Material;

struct HitRecord {
  real t = INFINITY;
  vec3 pos;
  vec2 uv;
  vec3 normal;
  bool front_face;

  Material *material;

  constexpr inline bool register_hit(const Ray &ray, const real t,
                                     const vec2 &uv, const vec3 &outward_normal,
                                     Material *material) {
    if (t >= this->t)
      return false;
    this->t = t;
    this->pos = ray.at(t);
    this->uv = uv;
    set_face_normal(ray, outward_normal);
    this->material = material;
    return true;
  }

  constexpr inline void set_face_normal(const Ray &ray,
                                        const vec3 &outward_normal) {
    front_face = glm::dot(ray.direction, outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};
