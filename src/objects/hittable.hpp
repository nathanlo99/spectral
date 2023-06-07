
#pragma once

#include "objects/bounding_box.hpp"
#include "util/ray.hpp"
#include "util/util.hpp"

struct HitRecord;

struct Hittable {
public:
  virtual bool hit(const Ray &r, real t_min, real t_max,
                   HitRecord &rec) const = 0;
  virtual BoundingBox bounding_box() const = 0;
};
