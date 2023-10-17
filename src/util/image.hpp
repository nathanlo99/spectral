
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

  constexpr inline Colour get(const int _row, const int _col) const {
    const int row = std::clamp(_row, 0, height - 1);
    const int col = std::clamp(_col, 0, width - 1);
    const int idx = row * width + col;
    const real r = data[3 * idx + 0] / 255.0;
    const real g = data[3 * idx + 1] / 255.0;
    const real b = data[3 * idx + 2] / 255.0;
    return Colour(r, g, b);
  }

  constexpr Colour get(const vec2 &uv) const {
    debug_assert(0.0 <= uv.x && uv.x <= 1.0, "u out of bounds");
    debug_assert(0.0 <= uv.y && uv.y <= 1.0, "v out of bounds");
    const real row = (1.0 - uv.y) * height;
    const real col = uv.x * width;

    const int row_int = static_cast<int>(row);
    const int col_int = static_cast<int>(col);
    const real row_frac = row - row_int;
    const real col_frac = col - col_int;

    // Bilinear interpolation
    const Colour top_left = get(row_int, col_int);
    const Colour top_right = get(row_int, col_int + 1);
    const Colour bottom_left = get(row_int + 1, col_int);
    const Colour bottom_right = get(row_int + 1, col_int + 1);

    return lerp(lerp(top_left, top_right, col_frac),
                lerp(bottom_left, bottom_right, col_frac), row_frac);
  }
};
