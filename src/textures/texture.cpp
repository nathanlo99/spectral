
#include "texture.hpp"

vec3 get_texture_value(const Texture &texture, const vec2 uv,
                       const vec3 &point) {
  return std::visit(
      [uv, point](const auto &texture) { return texture.value(uv, point); },
      texture);
}
