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

#ifndef MC_SAMPLES_COMMON_WASD_CAMERA_H_
#define MC_SAMPLES_COMMON_WASD_CAMERA_H_

#include <SDL.h>
#include <map>

#include "perspectiveCamera.h"

namespace mc { namespace samples {
  /**
   * Class representing perspective camera that is controlled by the WASD or
   * arrow keys and the mouse. This is essentially the same as the traditional
   * first-person-shooter with noclip mode turned on.
   */
  class WasdCamera : public PerspectiveCamera {
    private:
      std::map<SDL_Scancode, bool> m_depressed;
      glm::vec3 m_accel, m_vel;

    public:
      /**
       * Constructs a WasdCamera object. The WASD camera does not expose any
       * settings other than those provided by the PerspectiveCamera class from
       * which it inherits.
       *
       * \param fovy The y-axis viewing angle of the camera, in radians (not
       * degrees).
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
      WasdCamera(
          float fovy, float near = 0.1f, float far = 1000.0f,
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat());

      /**
       * Handles keyboard and mouse events that control this WASD camera.
       *
       * \param event The SDL event structure for the current event.
       * \return True if the given event was handled by the WASD camera, false
       * otherwise.
       */
      bool handleEvent(const SDL_Event &event);

      /**
       * Implements the tick() method to move the camera according to user
       * input.
       *
       * \param dt The time in seconds between the last tick and the current
       * tick.
       */
      void tick(float dt);
  };
} }

#endif
