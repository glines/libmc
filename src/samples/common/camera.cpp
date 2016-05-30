#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

namespace mc { namespace samples {
  Camera::Camera(const glm::vec3 &position, const glm::quat &orientation)
    : SceneObject(position, orientation)
  {
  }

  Camera::~Camera() {
  }

  glm::mat4 Camera::worldView(float alpha) const {
    glm::mat4 wv;

    // Translate the world so the camera is positioned in the center, and then
    // rotate the world to be aligned with the camera's orientation
    wv *= glm::mat4_cast(glm::inverse(this->orientation(alpha)));
    wv *= glm::translate(glm::mat4(), -1.0f * this->position(alpha));

    return wv;
  }
} }
