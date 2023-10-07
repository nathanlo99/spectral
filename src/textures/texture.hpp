#pragma once

#include <variant>

#include "util/util.hpp"

struct SolidColourTexture {
  vec3 colour;

  constexpr SolidColourTexture(const vec3 &colour) : colour(colour) {}

  constexpr vec3 value(const vec2 &, const vec3 &) const { return colour; }
};

using Texture = std::variant<SolidColourTexture>;
vec3 get_texture_value(const Texture &texture, const vec2 &uv,
                       const vec3 &point);
