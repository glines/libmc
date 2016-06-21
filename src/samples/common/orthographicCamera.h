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

#ifndef MC_SAMPLES_COMMON_OTHOGRAPHIC_CAMERA_H_
#define MC_SAMPLES_COMMON_OTHOGRAPHIC_CAMERA_H_

/**
 * \addtogroup samples
 * @{
 */

/**
 * \addtogroup common
 * @{
 */

#include "camera.h"

namespace mc { namespace samples {
  /**
   * A camera using orthographic projection.
   */
  class OrthographicCamera : public Camera {
    private:
      float m_left, m_right, m_bottom, m_top, m_near, m_far;
    public:
      /**
       * Constructs a camera with an orthographic projection. The left, right,
       * bottom, and top values determine the sides of the viewing volume. The
       * near and far plane values affect precision of the depth buffer.
       *
       * \param left The left side of the viewing volume.
       * \param right The right side of the viewing volume.
       * \param bottom The bottom side of the viewing volume.
       * \param top The top side of the viewing volume.
       * \param near The distance to the near plane of the viewing volume.
       * \param far The distance to the far plane of the viewing volume.
       * \param position The position of the camera.
       * \param orientation The orientation of the camera.
       */
      OrthographicCamera(float left, float right, float bottom, float top,
          float near = 0.1f, float far = 1000.0f,
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat());

      /**
       * Returns the orthographic projection transform matrix for this camera.
       * The aspect ratio given allows the camera to alter the projection to
       * preserve the aspect ratio.
       *
       * \param aspect The aspect ratio of the viewing window, which will be
       * preserved.
       * \param alpha The weight between keyframes in the animation of this
       * camera's projection.
       * \return The orthographic projection transform matrix for this camera.
       *
       * \todo Implement aspect ratio preservation for the orthographic camrea.
       * \todo The orthographic camera has no option for fitted versus cropped
       * aspect ratio.
       */
      glm::mat4 projection(float aspect, float alpha = 1.0f) const;
  };
} }

/** @} */

/** @} */

#endif
