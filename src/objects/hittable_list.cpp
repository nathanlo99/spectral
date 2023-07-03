
#include "hittable_list.hpp"
#include "hit_record.hpp"
#include "objects/bounding_box.hpp"

bool HittableList::hit(const Ray &ray, real t_min, real t_max,
                       HitRecord &record) const {
  HitRecord temp_record;
  bool hit_anything = false;
  real closest_so_far = t_max;

  for (const auto &object : objects) {
    real tmp = 0.0;
    if (object->bounding_box().does_hit(ray, t_min, t_max, tmp) &&
        object->hit(ray, tmp, closest_so_far, temp_record)) {
      hit_anything = true;
      closest_so_far = temp_record.t;
      record = temp_record;
    }
  }

  return hit_anything;
}
