#pragma once

#include <variant>

#include "util/util.hpp"

struct SolidColourTexture {
  Colour colour;

  constexpr SolidColourTexture(const Colour &colour) : colour(colour) {}

  constexpr Colour value(const vec2 &, const vec3 &) const { return colour; }
};

struct Texture {
  using TextureVariant = std::variant<SolidColourTexture>;
  TextureVariant texture;

  constexpr Texture(const Colour &colour)
      : texture(SolidColourTexture(colour)) {}

  template <typename T>
  constexpr explicit Texture(const T &texture) : texture(texture) {}

  constexpr Colour value(const vec2 &uv, const vec3 &point) const {
    return std::visit(
        [uv, point](const auto &texture) { return texture.value(uv, point); },
        texture);
  }
};
