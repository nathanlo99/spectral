
#include "diffuse_material.hpp"

#include "objects/hit_record.hpp"

bool DiffuseMaterial::scatter(RNG &random, const Ray &ray,
                              const HitRecord &record, vec3 &attenuation,
                              Ray &scattered) const {
  const vec3 scatter_direction = record.normal + random.random_unit_vec3();
  scattered = Ray(record.p, scatter_direction);
  attenuation = albedo;
  return true;
}
