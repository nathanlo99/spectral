
#pragma once

#include "util/image.hpp"
#include "util/ray.hpp"
#include "util/util.hpp"

struct Camera {
  vec3 pos;
  vec3 forward, right, up;

  real vertical_fov = 70; // Vertical field of view, in degrees
  real image_width = 640, image_height = 480;

  real m_pixel_size = 0.0;

  constexpr Camera(const vec3 &pos, const vec3 &look_at,
                   const vec3 &up = vec3(0.0, 1.0, 0.0))
      : pos(pos), forward(glm::normalize(look_at - pos)),
        right(glm::normalize(glm::cross(forward, up))),
        up(glm::cross(right, forward)) {

    fmt::println("Camera: pos     = {}\n\tforward = {}\n\tright   = "
                 "{}\n\tup      = {}",
                 pos, forward, right, up);

    update_constants();
  }

  template <typename Pixel>
  constexpr inline void set_output_image(const Image<Pixel> &image) {
    image_width = image.m_width;
    image_height = image.m_height;
    update_constants();
  }

  constexpr inline void set_fov(const real fov) {
    vertical_fov = fov;
    update_constants();
  }

  constexpr inline void update_constants() {
    const real near_plane_height =
        2.0f * std::tan(glm::radians(vertical_fov) * 0.5f);
    m_pixel_size = near_plane_height / image_height;
  }

  inline Ray get_ray(const vec2 pixel) const {
    const real x = pixel.x, y = pixel.y;
    const vec3 direction = forward +
                           (x - image_width / 2.0f) * m_pixel_size * right +
                           (image_height / 2.0f - y) * m_pixel_size * up;
    return Ray(pos, glm::normalize(direction));
  }
};
