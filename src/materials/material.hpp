
#pragma once

#include "util/random.hpp"
#include "util/ray.hpp"
#include "util/util.hpp"

struct HitRecord;

struct Material {
public:
  virtual bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
                       vec3 &attenuation, Ray &scattered) const = 0;
};
