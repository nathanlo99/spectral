
#pragma once

#include "util/cmf.hpp"
#include "util/piecewise_linear.hpp"
#include <array>

enum class SpectrumType { Reflectance, Illuminant };
enum ColourIndex : uint8_t {
  Black = 0b000,
  Blue = 0b001,
  Green = 0b010,
  Cyan = 0b011,
  Red = 0b100,
  Magenta = 0b101,
  Yellow = 0b110,
  White = 0b111,
};

extern std::array<std::array<PiecewiseLinear, 8>, 2> conversion_spectra;

inline std::array<real, 8> rgb_to_weights(const Colour &colour) {
  std::array<real, 8> result;
  // Sort the indices of the colour `colour`
  std::array<std::pair<real, size_t>, 3> sorted = {
      {std::make_pair(colour.r, 0), std::make_pair(colour.g, 1),
       std::make_pair(colour.b, 2)}};
  std::stable_sort(sorted.begin(), sorted.end());
  const size_t min_index = sorted[0].second;
  const size_t mid_index = sorted[1].second;
  const size_t max_index = sorted[2].second;

  const ColourIndex middle_index =
      ColourIndex((1 << max_index) | (1 << mid_index));

  result[ColourIndex::White] = colour[min_index];
  result[middle_index] = colour[mid_index] - colour[min_index];
  result[(1 << max_index)] = colour[max_index] - colour[mid_index];

  return result;
}

template <SpectrumType spectrum_type>
inline real rgb_to_spectral(const Colour &colour, const real wavelength) {
  const std::array<real, 8> weights = rgb_to_weights(colour);
  real result = 0.0;
  for (size_t i = 0; i < 8; ++i) {
    if (weights[i] <= 0)
      continue;
    const auto &piecewise_linear =
        conversion_spectra[static_cast<size_t>(spectrum_type)][i];
    result += piecewise_linear.at(wavelength) * weights[i];
  }
  return glm::clamp(result, 0.0, 1.0);
}
