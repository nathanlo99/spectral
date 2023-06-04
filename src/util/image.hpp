
#pragma once

#include "util/util.hpp"

using RGBByte = std::array<unsigned char, 3>;

constexpr inline real gamma_correct_real(const real d) {
  constexpr real gamma = 2.2, gamma_exp = 1.0 / gamma;
  return std::pow(d, gamma_exp);
}

constexpr inline unsigned char to_byte(const real d) { return 255.99 * d; }

constexpr inline RGBByte to_bytes(const vec3 &pixel) {
  RGBByte result;
  result[0] = to_byte(std::clamp<real>(pixel[0], 0.0, 1.0));
  result[1] = to_byte(std::clamp<real>(pixel[1], 0.0, 1.0));
  result[2] = to_byte(std::clamp<real>(pixel[2], 0.0, 1.0));
  return result;
}

template <bool gamma_correct = true>
constexpr inline vec3 gamma_correct_pixel(const vec3 &pixel) {
  if constexpr (!gamma_correct)
    return pixel;
  vec3 result;
  result[0] = gamma_correct_real(pixel[0]);
  result[1] = gamma_correct_real(pixel[1]);
  result[2] = gamma_correct_real(pixel[2]);
  return result;
}

template <typename Pixel> struct Image {
  size_t m_width = 0;
  size_t m_height = 0;
  std::vector<Pixel> m_pixels;

  constexpr Image(const size_t width, const size_t height)
      : m_width(width), m_height(height), m_pixels(width * height) {}

  constexpr void set_pixel(const size_t row, const size_t col,
                           const Pixel &pixel) {
    debug_assert(row < m_height, "row ({}) >= m_height ({})", row, m_height);
    debug_assert(col < m_width, "col ({}) >= m_width ({})", col, m_width);
    m_pixels[row * m_width + col] = pixel;
  }

  template <typename Sample>
  void add_pixel_sample(const size_t row, const size_t col,
                        const Sample &sample) {
    debug_assert(row < m_height, "row ({}) >= m_height ({})", row, m_height);
    debug_assert(col < m_width, "col ({}) >= m_width ({})", col, m_width);
    m_pixels[row * m_width + col].add_sample(sample);
  }

  // TODO: Consider allowing global tone-mapping:
  // https://64.github.io/tonemapping
  template <bool gamma_correct = true>
  void write_png(const std::string_view &filename) const {
    std::vector<unsigned char> gamma_corrected_data(3 * m_width * m_height);
    for (size_t i = 0; i < m_width * m_height; ++i) {
      const RGBByte rgb_pixel =
          to_bytes(gamma_correct_pixel<gamma_correct>(m_pixels[i].to_pixel()));
      gamma_corrected_data[3 * i + 0] = rgb_pixel[0];
      gamma_corrected_data[3 * i + 1] = rgb_pixel[1];
      gamma_corrected_data[3 * i + 2] = rgb_pixel[2];
    }

    const int result = stbi_write_png(filename.data(), m_width, m_height, 3,
                                      gamma_corrected_data.data(), 3 * m_width);
    debug_assert(result != 0, "write_png({}) failed: is the location writable?",
                 filename);
  }
};

struct RGBPixel {
  vec3 m_mean = vec3(0.0);
  real m_num_samples = 0;

  constexpr RGBPixel() {}
  constexpr RGBPixel(const vec3 &data) : m_mean(data), m_num_samples(1) {}

  constexpr inline void add_sample(const vec3 &_sample) {
    const vec3 sample = remove_nans(_sample);
    m_num_samples += 1;
    m_mean += (sample - m_mean) / m_num_samples;
  }
  constexpr inline vec3 to_pixel() const { return m_mean; }
};

struct RGBVariancePixel {
  vec3 m_mean;
  vec3 m_variance = vec3(0.0);
  real m_num_samples = 0;

  constexpr RGBVariancePixel(const vec3 &data = vec3(0.0, 0.0, 0.0))
      : m_mean(data), m_variance(0.0) {}
  constexpr inline void add_sample(const vec3 &_sample) {
    const vec3 sample = remove_nans(_sample);
    if (m_num_samples == 0) {
      m_mean = sample;
      m_num_samples = 1.0;
      return;
    }
    const vec3 old_mean = m_mean;
    m_num_samples += 1;
    m_mean += (sample - m_mean) / m_num_samples;
    m_variance += (sample - old_mean) * (sample - m_mean);
  }
  constexpr inline vec3 to_pixel() const {
    if (m_num_samples == 0)
      return vec3(0.0);
    return m_variance / m_num_samples;
  }
};

using RGBImage = Image<RGBPixel>;
using RGBVarianceImage = Image<RGBVariancePixel>;
