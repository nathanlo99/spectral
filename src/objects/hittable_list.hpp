
#pragma once

#include "hittable.hpp"
#include "objects/bounding_box.hpp"
#include "util/ray.hpp"
#include "util/util.hpp"

#include <memory>
#include <vector>

struct HitRecord;

struct HittableList : public Hittable {
  std::vector<std::shared_ptr<Hittable>> objects;
  BoundingBox box;

  HittableList() {}
  HittableList(std::shared_ptr<Hittable> object) { add(object); }
  virtual ~HittableList() {}

  constexpr void clear() { objects.clear(); }
  void add(std::shared_ptr<Hittable> object) {
    objects.push_back(object);
    box.include(object->bounding_box());
  }

  virtual bool hit(const Ray &ray, real t_min, real t_max,
                   HitRecord &record) const override;
  virtual BoundingBox bounding_box() const override { return box; }
};
