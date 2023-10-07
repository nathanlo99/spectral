
#pragma once

#include <variant>

#include "util/random.hpp"
#include "util/ray.hpp"
#include "util/util.hpp"

struct HitRecord;

struct DielectricMaterial {
  const Colour albedo;
  const real refractive_index;

  constexpr DielectricMaterial(const Colour &albedo,
                               const real refractive_index)
      : albedo(albedo), refractive_index(refractive_index) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               Colour &attenuation, Ray &scattered) const;
};

struct DiffuseMaterial {
  const Colour albedo;

  constexpr DiffuseMaterial(const Colour &albedo) : albedo(albedo) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               Colour &attenuation, Ray &scattered) const;
};

struct ReflectiveMaterial {
  const Colour albedo;
  const real fuzz;

  constexpr ReflectiveMaterial(const Colour &albedo, const real fuzz)
      : albedo(albedo), fuzz(std::clamp<real>(fuzz, 0.0, 1.0)) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               Colour &attenuation, Ray &scattered) const;
};

struct Material {
  using MaterialVariant =
      std::variant<DielectricMaterial, DiffuseMaterial, ReflectiveMaterial>;
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
};
