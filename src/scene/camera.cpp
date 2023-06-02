
#include "camera.hpp"

Camera::Camera(const vec3 &pos, const vec3 &look_at, const vec3 &up)
    : pos(pos), forward(glm::normalize(look_at - pos)),
      right(glm::normalize(glm::cross(forward, up))),
      up(glm::cross(right, forward)) {

  fmt::println("Camera: pos     = {}\n\tforward = {}\n\tright   = "
               "{}\n\tup      = {}",
               pos, forward, right, up);

  update_constants();
}

void Camera::set_fov(const real fov) {
  vertical_fov = fov;
  update_constants();
}

void Camera::update_constants() {
  const real near_plane_height =
      2.0f * std::tan(glm::radians(vertical_fov) * 0.5f);
  m_pixel_size = near_plane_height / image_height;
}

Ray Camera::get_ray(const vec2 pixel) const {
  const real x = pixel.x, y = pixel.y;
  const vec3 direction = forward +
                         (x - image_width / 2.0f) * m_pixel_size * right +
                         (image_height / 2.0f - y) * m_pixel_size * up;
  return Ray(pos, direction);
}
