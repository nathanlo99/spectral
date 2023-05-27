
#pragma once

#include "util.hpp"

using RGBByte = std::array<unsigned char, 3>;

constexpr inline float gamma_correct_real(const real d) {
  constexpr real gamma = 2.2, gamma_exp = 1.0 / gamma;
  return std::pow(d, gamma_exp);
}

constexpr inline unsigned char to_byte(const real d) { return 255.0 * d; }

inline RGBByte straight_to_pixel(const vec3 &pixel) {
  RGBByte result;
  result[0] = to_byte(std::clamp<real>(pixel[0], 0.0, 1.0));
  result[1] = to_byte(std::clamp<real>(pixel[1], 0.0, 1.0));
  result[2] = to_byte(std::clamp<real>(pixel[2], 0.0, 1.0));
  return result;
}

inline RGBByte default_to_pixel(const vec3 &pixel) {
  RGBByte result;
  result[0] = to_byte(gamma_correct_real(std::clamp<real>(pixel[0], 0.0, 1.0)));
  result[1] = to_byte(gamma_correct_real(std::clamp<real>(pixel[1], 0.0, 1.0)));
  result[2] = to_byte(gamma_correct_real(std::clamp<real>(pixel[2], 0.0, 1.0)));
  return result;
}

template <typename Pixel> struct Image {
  size_t m_width;
  size_t m_height;
  std::vector<Pixel> m_pixels;

  Image(const size_t width, const size_t height)
      : m_width(width), m_height(height), m_pixels(width * height, Pixel()) {}

  void set_pixel(const size_t row, const size_t col, const Pixel &pixel) {
    assert(row < m_height);
    assert(col < m_width);
    m_pixels[row * m_width + col] = pixel;
  }

  // TODO: Consider allowing global tone-mapping:
  // https://64.github.io/tonemapping
  void write_png(const std::string_view &filename,
                 const std::function<RGBByte(const Pixel &pixel)> &to_pixel =
                     default_to_pixel) {
    std::vector<unsigned char> gamma_corrected_data(3 * m_width * m_height);
    for (size_t i = 0; i < m_width * m_height; ++i) {
      const RGBByte rgb_pixel = to_pixel(m_pixels[i]);
      gamma_corrected_data[3 * i + 0] = rgb_pixel[0];
      gamma_corrected_data[3 * i + 1] = rgb_pixel[1];
      gamma_corrected_data[3 * i + 2] = rgb_pixel[2];
    }

    const int result = stbi_write_png(filename.data(), m_width, m_height, 3,
                                      gamma_corrected_data.data(), 3 * m_width);
    debug_assert(result != 0, "write_png({}) failed", filename);
  }
};

using RGBImage = Image<glm::vec3>;
