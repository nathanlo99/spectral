
#pragma once

#include "util/ray.hpp"
#include "util/util.hpp"

struct HitRecord;

struct Hittable {
public:
  virtual bool hit(const Ray &r, real t_min, real t_max,
                   HitRecord &rec) const = 0;
};
