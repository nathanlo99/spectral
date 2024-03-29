
#include "scene.hpp"

#include "materials/material.hpp"
#include "objects/hit_record.hpp"
#include "util/spectral_conversion.hpp"

Colour Scene::ray_colour(RNG &random, const size_t max_depth,
                         const Ray &ray) const {
  Colour colour(1.0, 1.0, 1.0);
  Ray current_ray = ray;
  for (size_t depth = 0; depth < max_depth; ++depth) {
    HitRecord record;
    if (!world.hit(current_ray, 0.0001, INFINITY, record))
      return colour * get_background_colour(current_ray);

    Ray scattered;
    Colour attenuation;
    // TODO: Return emitted light
    if (!record.material->scatter(random, current_ray, record, attenuation,
                                  scattered))
      return Colour(0.0, 0.0, 0.0);

    colour *= attenuation;
    current_ray = scattered;
  }

  return Colour(0.0, 0.0, 0.0);
}

real Scene::get_intensity(RNG &random, const size_t max_depth,
                          const Ray &ray) const {
  const real wavelength = ray.wavelength;
  real intensity = 1.0;
  Ray current_ray = ray;
  for (size_t depth = 0; depth < max_depth; ++depth) {
    HitRecord record;
    if (!world.hit(current_ray, 0.0001, INFINITY, record))
      return intensity * rgb_to_spectral<SpectrumType::Illuminant>(
                             get_background_colour(current_ray), wavelength);

    Ray scattered;
    Colour attenuation;
    // TODO: Return emitted light
    if (!record.material->scatter(random, current_ray, record, attenuation,
                                  scattered))
      return 0.0;

    intensity *=
        rgb_to_spectral<SpectrumType::Reflectance>(attenuation, wavelength);
    current_ray = scattered;
  }

  return intensity;
}
