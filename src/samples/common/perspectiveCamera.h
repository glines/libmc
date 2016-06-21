/*
 * Copyright (c) 2016 Jonathan Glines
 * Jonathan Glines <jonathan@glines.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef MC_SAMPLES_COMMON_PERSPECTIVE_CAMERA_H_
#define MC_SAMPLES_COMMON_PERSPECTIVE_CAMERA_H_

/**
 * \addtogroup samples
 */

/**
 * \addtogroup common
 */

#include "camera.h"

namespace mc { namespace samples {
  /**
   * A camera object with perspective projection.
   */
  class PerspectiveCamera : public Camera {
    private:
      float m_fovy, m_prevFovy,
            m_near, m_far;
    public:
      /**
       * Constructs a perspective camera object with the given parameters.
       *
       * \param fovy The y-axis viewing angle of the camera, in radians.
       * \param near The distance of the near plane of the camera. For the most
       * depth precision, this value should be as large as is deemed acceptable
       * for the given application.
       * \param far The distance of the farplane of the camera. In practice,
       * this value can be as large as necessary with no repercussion.
       * \param position The camera position.
       * \param orientation The camera orientation. For best results, this
       * orientation should be pointed directly at the follow point with the
       * z-axis pointing up.
       */
      PerspectiveCamera(float fovy, float near = 0.1f, float far = 1000.0f,
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat());
      virtual ~PerspectiveCamera();

      glm::mat4 projection(float aspect, float alpha = 1.0f) const;
  };
} }

/** @} */

/** @} */

#endif
