#pragma once

#include <variant>

#include "util/util.hpp"

struct SolidColourTexture {
  Colour colour;

  constexpr SolidColourTexture(const Colour &colour) : colour(colour) {}

  constexpr Colour value(const vec2 &, const vec3 &) const { return colour; }
};

using Texture = std::variant<SolidColourTexture>;
Colour get_texture_value(const Texture &texture, const vec2 &uv,
                         const vec3 &point);
