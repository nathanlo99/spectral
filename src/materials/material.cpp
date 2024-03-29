
#include "material.hpp"

#include "objects/hit_record.hpp"

real schlick_reflection_probability(const real cos_theta,
                                    const real ior_ratio) {
  const real r0 = (1.0 - ior_ratio) / (1.0 + ior_ratio);
  const real r0_squared = r0 * r0;
  return r0_squared + (1.0 - r0_squared) * std::pow(1.0 - cos_theta, 5.0);
}

bool DielectricMaterial::scatter(RNG &random, const Ray &ray,
                                 const HitRecord &record, Colour &attenuation,
                                 Ray &scattered) const {

  attenuation = albedo.value(record.uv, record.pos);

  const real ior_ratio =
      record.front_face ? (1.0 / refractive_index) : refractive_index;
  const real cos_theta = glm::dot(-ray.direction, record.normal);
  const real sin_theta = std::sqrt<real>(1.0 - cos_theta * cos_theta);
  const bool should_reflect =
      ior_ratio * sin_theta > 1.0 ||
      random.random_bool(schlick_reflection_probability(cos_theta, ior_ratio));
  const vec3 scattered_direction =
      should_reflect ? glm::reflect(ray.direction, record.normal)
                     : glm::refract(ray.direction, record.normal, ior_ratio);

  scattered = Ray(record.pos, scattered_direction);
  return true;
}

bool DiffuseMaterial::scatter(RNG &random, const Ray &, const HitRecord &record,
                              Colour &attenuation, Ray &scattered) const {
  const vec3 scatter_direction = record.normal + random.random_unit_vec3();
  scattered = Ray(record.pos, scatter_direction);
  attenuation = albedo.value(record.uv, record.pos);
  return true;
}

bool ReflectiveMaterial::scatter(RNG &random, const Ray &ray,
                                 const HitRecord &record, Colour &attenuation,
                                 Ray &scattered) const {
  const vec3 reflected = glm::reflect(ray.direction, record.normal);
  scattered =
      Ray(record.pos, reflected + fuzz * random.random_in_unit_sphere());
  attenuation = albedo.value(record.uv, record.pos);
  return glm::dot(scattered.direction, record.normal) > 0;
}

bool SpectralMaterial::scatter(RNG &random, const Ray &ray,
                               const HitRecord &record, Colour &attenuation,
                               Ray &scattered) const {
  const real ior_ratio = get_ior(ray);
  const real cos_theta = glm::dot(-ray.direction, record.normal);
  const real sin_theta = std::sqrt<real>(1.0 - cos_theta * cos_theta);
  const bool should_reflect =
      ior_ratio * sin_theta > 1.0 ||
      random.random_bool(schlick_reflection_probability(cos_theta, ior_ratio));
  const vec3 scattered_direction =
      should_reflect ? glm::reflect(ray.direction, record.normal)
                     : glm::refract(ray.direction, record.normal, ior_ratio);

  scattered = Ray(record.pos, scattered_direction);
  attenuation = albedo.value(record.uv, record.pos);
  return true;
}
