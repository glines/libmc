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

#ifndef MC_SAMPLES_COMMON_ARCBALL_CAMERA_H_
#define MC_SAMPLES_COMMON_ARCBALL_CAMERA_H_

#include <SDL.h>

#include "perspectiveCamera.h"

namespace mc { namespace samples {
  /**
   * This class implements arcball controls for a camera by reading input from
   * the mouse.
   */
  class ArcballCamera : public PerspectiveCamera {
    private:
      glm::vec3 m_followPoint;
      glm::vec3 m_arcballInitial, m_arcballFinal;
      glm::vec3 m_initialPosition;
      glm::quat m_initialOrientation;
      float m_arcballRadius;
      bool m_dragStarted;

      /**
       * Calculates the position on the surface of the arcball sphere given a
       * click position in normalized view coordinates.
       */
      glm::vec3 m_arcballPosition(float x, float y);

    public:
      ArcballCamera(float fovy, float near = 0.1f, float far = 1000.0f,
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat(),
          const glm::vec3 &followPoint = glm::vec3(0.0f, 0.0f, 0.0f));

      bool mouseButtonEvent(const SDL_MouseButtonEvent &event,
          int windowWidth, int windowHeight);
      bool mouseMotionEvent(const SDL_MouseMotionEvent &event,
          int windowWidth, int windowHeight);

      void tick(float dt);

      void draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug) const;
  };
} }

#endif
