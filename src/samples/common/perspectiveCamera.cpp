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

#include <cmath>
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
