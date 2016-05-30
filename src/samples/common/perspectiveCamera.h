#ifndef MC_SAMPLES_COMMON_PERSPECTIVE_CAMERA_H_
#define MC_SAMPLES_COMMON_PERSPECTIVE_CAMERA_H_

#include "camera.h"

namespace mc { namespace samples {
  class PerspectiveCamera : public Camera {
    private:
      float m_fovy, m_prevFovy,
            m_near, m_far;
    public:
      PerspectiveCamera(float fovy, float near = 0.1f, float far = 1000.0f,
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat());
      virtual ~PerspectiveCamera();

      glm::mat4 projection(float aspect, float alpha = 1.0f) const;
  };
} }

#endif
