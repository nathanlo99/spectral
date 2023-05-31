
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

  void clear() { objects.clear(); }
  void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

  virtual bool hit(const Ray &ray, real t_min, real t_max,
                   HitRecord &record) const override {
    HitRecord temp_record;
    bool hit_anything = false;
    real closest_so_far = t_max;

    for (const auto &object : objects) {
      if (object->hit(ray, t_min, closest_so_far, temp_record)) {
        hit_anything = true;
        closest_so_far = temp_record.t;
        record = temp_record;
      }
    }

    return hit_anything;
  }

public:
  std::vector<std::shared_ptr<Hittable>> objects;
};
