
#pragma once

#include "util/cmf.hpp"
#include "util/piecewise_linear.hpp"
#include "util/util.hpp"

using RGBByte = std::array<uint8_t, 3>;

constexpr inline real gamma_correct_real(const real d) {
  constexpr real gamma = 2.2, gamma_exp = 1.0 / gamma;
  return std::pow(d, gamma_exp);
}

constexpr inline uint8_t to_byte(const real d) { return 255.99 * d; }

constexpr inline RGBByte to_bytes(const Colour &pixel) {
  RGBByte result;
  result[0] = to_byte(std::clamp<real>(pixel[0], 0.0, 1.0));
  result[1] = to_byte(std::clamp<real>(pixel[1], 0.0, 1.0));
  result[2] = to_byte(std::clamp<real>(pixel[2], 0.0, 1.0));
  return result;
}

template <bool gamma_correct = true>
constexpr inline Colour gamma_correct_pixel(const Colour &pixel) {
  if constexpr (!gamma_correct)
    return pixel;
  Colour result;
  result[0] = gamma_correct_real(pixel[0]);
  result[1] = gamma_correct_real(pixel[1]);
  result[2] = gamma_correct_real(pixel[2]);
  return result;
}

template <typename Pixel> struct Image {
  const size_t m_width;
  const size_t m_height;
  std::vector<Pixel> m_pixels;

  constexpr Image(const size_t width, const size_t height)
      : m_width(width), m_height(height), m_pixels(width * height) {}

  constexpr void set_pixel(const size_t row, const size_t col,
                           const Pixel &pixel) {
    debug_assert(row < m_height, "row ({}) >= m_height ({})", row, m_height);
    debug_assert(col < m_width, "col ({}) >= m_width ({})", col, m_width);
    m_pixels[row * m_width + col] = pixel;
  }

  void add_pixel_sample(const size_t row, const size_t col,
                        const typename Pixel::sample_t &sample) {
    debug_assert(row < m_height, "row ({}) >= m_height ({})", row, m_height);
    debug_assert(col < m_width, "col ({}) >= m_width ({})", col, m_width);
    m_pixels[row * m_width + col].add_sample(sample);
  }

  // TODO: Consider allowing global tone-mapping:
  // https://64.github.io/tonemapping
  template <bool gamma_correct = true>
  void write_png(const std::string_view &filename) const {
    std::vector<uint8_t> gamma_corrected_data(3 * m_width * m_height);
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
  using sample_t = Colour;
  Colour m_mean = Colour(0.0, 0.0, 0.0);
  real m_num_samples = 0;

  constexpr RGBPixel() {}
  constexpr RGBPixel(const Colour &data) : m_mean(data), m_num_samples(1) {}

  constexpr inline void add_sample(const sample_t &_sample) {
    const Colour sample = remove_nans(_sample);
    m_num_samples += 1;
    m_mean += (sample - m_mean) / m_num_samples;
  }
  constexpr inline Colour to_pixel() const { return m_mean; }
};

struct RGBVariancePixel {
  using sample_t = Colour;
  Colour m_mean = Colour(0.0);
  Colour m_variance = Colour(0.0);
  real m_num_samples = 0;

  constexpr RGBVariancePixel(const Colour &data = Colour(0.0, 0.0, 0.0))
      : m_mean(data), m_variance(0.0) {}
  constexpr inline void add_sample(const sample_t &_sample) {
    const Colour sample = remove_nans(_sample);
    const Colour old_mean = m_mean;
    m_num_samples += 1;
    m_mean += (sample - m_mean) / m_num_samples;
    m_variance += (sample - old_mean) * (sample - m_mean);
  }
  constexpr inline Colour to_pixel() const {
    if (m_num_samples == 0)
      return Colour(0.0);
    return m_variance / m_num_samples;
  }
};

struct SpectralPixel {
  struct sample_t {
    const real wavelength;
    const real value;
    constexpr sample_t(const real wavelength, const real value)
        : wavelength(wavelength), value(value) {}
  };
  static constexpr real min_wavelength = 400.0, max_wavelength = 700.0;
  PiecewiseLinear m_function;

  constexpr SpectralPixel() = default;

  constexpr inline void add_sample(const sample_t &sample) {
    debug_assert(min_wavelength <= sample.wavelength &&
                     sample.wavelength <= max_wavelength,
                 "wavelength ({}) not in [{}, {}]", sample.wavelength,
                 min_wavelength, max_wavelength);
    m_function.add_point(sample.wavelength, sample.value);
  }

  inline Colour to_pixel() const {
    const auto combine = [](const PiecewiseLinear &intensities,
                            const PiecewiseLinear &cmf_component) {
      return intensities.dot_product(cmf_component, min_wavelength,
                                     max_wavelength) /
             (max_wavelength - min_wavelength);
    };

    const auto &cmf = ColourMatchingFunction::get();
    const real X = combine(m_function, cmf.X);
    const real Y = combine(m_function, cmf.Y);
    const real Z = combine(m_function, cmf.Z);
    const real R = 3.2404542 * X - 1.5371385 * Y - 0.4985314 * Z;
    const real G = -0.9692660 * X + 1.8760108 * Y + 0.0415560 * Z;
    const real B = 0.0556434 * X - 0.2040259 * Y + 1.0572252 * Z;
    return Colour(R, G, B);
  }
};

using RGBImage = Image<RGBPixel>;
using RGBVarianceImage = Image<RGBVariancePixel>;
