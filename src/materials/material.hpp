
#pragma once

#include <variant>

#include "util/ior_function.hpp"
#include "util/random.hpp"
#include "util/ray.hpp"
#include "util/util.hpp"

#include "textures/texture.hpp"

struct HitRecord;

struct DielectricMaterial {
  const Texture albedo;
  const real refractive_index;

  constexpr DielectricMaterial(const Texture &albedo,
                               const real refractive_index)
      : albedo(albedo), refractive_index(refractive_index) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               Colour &attenuation, Ray &scattered) const;
  real get_ior(const Ray &) const { return refractive_index; }
};

struct DiffuseMaterial {
  const Texture albedo;

  constexpr explicit DiffuseMaterial(const Texture &albedo) : albedo(albedo) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               Colour &attenuation, Ray &scattered) const;
  real get_ior(const Ray &) const { return 1.0; }
};

struct ReflectiveMaterial {
  const Texture albedo;
  const real fuzz;

  constexpr ReflectiveMaterial(const Texture &albedo, const real fuzz)
      : albedo(albedo), fuzz(std::clamp<real>(fuzz, 0.0, 1.0)) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               Colour &attenuation, Ray &scattered) const;
  real get_ior(const Ray &) const { return 1.0; }
};

struct SpectralMaterial {
  const Texture albedo;
  const IORFunction ior_function;

  constexpr SpectralMaterial(const Texture &albedo,
                             const IORFunction &ior_function)
      : albedo(albedo), ior_function(ior_function) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               Colour &attenuation, Ray &scattered) const;
  real get_ior(const Ray &ray) const { return ior_function.get_ior(ray); }
};

struct Material {
  using MaterialVariant = std::variant<DielectricMaterial, DiffuseMaterial,
                                       ReflectiveMaterial, SpectralMaterial>;
  MaterialVariant material;

  template <typename T>
  constexpr Material(const T &material) : material(material) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               Colour &attenuation, Ray &scattered) const {
    return std::visit(
        [&random, &ray, &record, &attenuation,
         &scattered](const auto &material) {
          return material.scatter(random, ray, record, attenuation, scattered);
        },
        material);
  }
  real get_ior(const Ray &ray) const {
    return std::visit(
        [&ray](const auto &material) { return material.get_ior(ray); },
        material);
  }
};
