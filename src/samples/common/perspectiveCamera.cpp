#include <glm/gtc/matrix_transform.hpp>

#include "perspectiveCamera.h"

namespace mc { namespace samples {
  PerspectiveCamera::PerspectiveCamera(float fovy, float near, float far,
      const glm::vec3 &position, const glm::quat &orientation)
    : Camera(position, orientation),
      m_fovy(fovy), m_prevFovy(fovy),
      m_near(near), m_far(far)
  {
  }

  PerspectiveCamera::~PerspectiveCamera() {
  }

  glm::mat4 PerspectiveCamera::projection(
      float aspect, float alpha) const
  {
    // Linearly interpolate changes in FOV between ticks
    float fovy = (1.0f - alpha) * m_prevFovy + alpha * m_fovy;

    return glm::perspective(fovy, aspect, m_near, m_far);
  }
} }
