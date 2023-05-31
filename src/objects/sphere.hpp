
#pragma once

#include "hittable.hpp"

#include "util/util.hpp"

struct Sphere : Hittable {
  vec3 center = vec3(0.0);
  real radius = 1.0;

  Sphere(vec3 center, real radius) : center(center), radius(radius){};
  virtual ~Sphere() {}

  virtual bool hit(const Ray &ray, real t_min, real t_max,
                   HitRecord &record) const override {
    const vec3 oc = ray.origin - center;
    const real a = glm::dot(ray.direction, ray.direction);
    const real half_b = glm::dot(oc, ray.direction);
    const real c = glm::dot(oc, oc) - radius * radius;

    const real discriminant = half_b * half_b - a * c;
    if (discriminant < 0)
      return false;
    const real sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    real root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
      root = (-half_b + sqrtd) / a;
      if (root < t_min || t_max < root)
        return false;
    }

    record.t = root;
    record.p = ray.at(record.t);
    vec3 outward_normal = (record.p - center) / radius;
    record.set_face_normal(ray, outward_normal);

    return true;
  }
};
