
#pragma once

#include "hittable.hpp"
#include "util/ray.hpp"
#include "util/util.hpp"

#include <memory>
#include <vector>

struct HittableList : Hittable {
  HittableList() {}
  HittableList(std::shared_ptr<Hittable> object) { add(object); }
  virtual ~HittableList() {}

  constexpr void clear() { objects.clear(); }
  void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

  virtual bool hit(const Ray &ray, real t_min, real t_max,
                   HitRecord &record) const override;

public:
  std::vector<std::shared_ptr<Hittable>> objects;
};
