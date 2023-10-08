
#pragma once

#include "util/util.hpp"

struct Image {
  int width, height;
  unsigned char *data;

  Image(const std::string_view filename) {
    data = stbi_load(filename.data(), &width, &height, nullptr, 3);
    debug_assert(data != nullptr, "Failed to load image: {}", filename);
  }
  ~Image() { free(data); }

  Image(const Image &image) {
    width = image.width;
    height = image.height;
    data = new unsigned char[3 * width * height];
    memcpy(data, image.data, 3 * width * height);
  }
  Image &operator=(const Image &image) {
    width = image.width;
    height = image.height;
    data = static_cast<unsigned char *>(realloc(data, 3 * width * height));
    memcpy(data, image.data, 3 * width * height);
    return *this;
  }

  constexpr Colour get(const int row, const int col) const {
    debug_assert(0 <= row && row < height, "Row out of bounds");
    debug_assert(0 <= col && col < width, "Column out of bounds");
    const int idx = row * width + col;
    const unsigned char r = data[3 * idx + 0];
    const unsigned char g = data[3 * idx + 1];
    const unsigned char b = data[3 * idx + 2];
    return Colour(r / 255.0, g / 255.0, b / 255.0);
  }

  constexpr Colour get(const vec2 &uv) const {
    debug_assert(0.0 <= uv.x && uv.x <= 1.0, "u out of bounds");
    debug_assert(0.0 <= uv.y && uv.y <= 1.0, "v out of bounds");
    const int row = (1.0 - uv.y) * height;
    const int col = uv.x * width;
    return get(row, col);
  }
};
