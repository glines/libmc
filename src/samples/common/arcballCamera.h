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
      /**
       * Constructs an arcball camera object with the given parameters. Since
       * all arcball cameras are perspective cameras, these parameters are the
       * same as in the constructor for PerspectiveCamera.
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
       * \param followPoint The point which the arcball camera is always
       * facing.
       */
      ArcballCamera(float fovy, float near = 0.1f, float far = 1000.0f,
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat(),
          const glm::vec3 &followPoint = glm::vec3(0.0f, 0.0f, 0.0f));

      /**
       * The arcball camera consumes mouse button events to allow the user to
       * control the camera.
       */
      bool mouseButtonEvent(const SDL_MouseButtonEvent &event,
          int windowWidth, int windowHeight);
      /**
       * The arcball camera consumes mouse motion events to allow the user to
       * control the camera.
       *
       * \param event SDL structure describing this mouse motion event.
       * \param windowWidth The width in pixels of the window receiving the
       * event.
       * \param windowHeight The height in pixels of the window receiving the
       * event.
       * \return True if the arcball camera handles this event, false
       * otherwise.
       */
      bool mouseMotionEvent(const SDL_MouseMotionEvent &event,
          int windowWidth, int windowHeight);

      /**
       * Arcball camera implement the tick event for smooth camera animations.
       *
       * \param dt The delta time in seconds between the previous simulation
       * tick and this new tick.
       *
       * \todo I don't think that any smooth camera animations are being done
       * at this time.
       */
      void tick(float dt);

      /**
       * Draws debugging lines for visualizing the movement of the arcball
       * camera.
       *
       * \param modelWorld The model-space to world-space transform for this
       * scene object's position and orientation.
       * \param worldView The world-space to view-space transform for the
       * position and orientation of the camera currently being used.
       * \param projection The view-space to projection-space transform for
       * the camera that is currently being used.
       * \param alpha The simulation keyframe weight for animating this object
       * between keyframes.
       * \param debug Flag indicating whether or not debug information is to be
       * drawn.
       *
       * \todo Arcball camera debugging lines are not being drawn at the
       * momemnt.
       */
      void draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug) const;
  };
} }

#endif
