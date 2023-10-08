#pragma once

#include <variant>

#include "util/image.hpp"
#include "util/util.hpp"

struct SolidColourTexture {
  Colour colour;

  constexpr SolidColourTexture(const Colour &colour) : colour(colour) {}

  constexpr Colour value(const vec2 &, const vec3 &) const { return colour; }
};

struct ImageTexture {
  Image image;

  ImageTexture(const std::string_view &filename) : image(filename) {}

  constexpr Colour value(const vec2 &uv, const vec3 &) const {
    return image.get(uv);
  }
};

struct Texture {
  using TextureVariant = std::variant<SolidColourTexture, ImageTexture>;
  TextureVariant texture;

  constexpr Texture(const Colour &colour)
      : texture(SolidColourTexture(colour)) {}

  template <typename T>
  constexpr Texture(const T &texture) : texture(texture) {}

  constexpr Colour value(const vec2 &uv, const vec3 &point) const {
    return std::visit(
        [uv, point](const auto &texture) { return texture.value(uv, point); },
        texture);
  }
};
