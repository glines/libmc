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

#ifndef MC_SAMPLES_COMMON_CAMERA_H_
#define MC_SAMPLES_COMMON_CAMERA_H_

#include <glm/glm.hpp>

#include "sceneObject.h"

namespace mc { namespace samples {
  /**
   * Abstract class defining a camera in a 3D graphics scene.
   *
   * The camera can generate the worldView matrix transform from the camera's
   * current position and orientation and the frame interpolating value alpha.
   * Derived classes are to generate an appropriate projection matrix
   * transform.
   */
  class Camera : public SceneObject {
    public:
      /**
       * Constructs a camera object with the given position and orientation.
       * Since Camera is a pure-virtual class, this constructor does not
       * actually provide enough parameters to define a camera. See one of the
       * derived classes for a complete camera definition.
       *
       * \param position The position of the camera object.
       * \param orientation The orientation of the camera object.
       */
      Camera(const glm::vec3 &position, const glm::quat &orientation);
      /**
       * Destroys the camera object. Since Camera is a polymorphic class, this
       * destructor is virtual.
       */
      virtual ~Camera();

      /**
       * Returns the transform from world space to view space based on the
       * camera's current position and orientation.
       *
       * \param alpha The interpolation weight between the last tick and the
       * current tick.
       * \return The world-space to view-space matrix transform for this
       * camera.
       */
      virtual glm::mat4 worldView(float alpha = 1.0) const;

      /**
       * Derived camera classes must implement this method to return a
       * projection transform for the camera.
       *
       * \param aspect The aspect ratio of the viewport, measured in width over
       * height.
       * \param alpha The interpolation weight between the last tick and the
       * current tick.
       * \return The projection matrix transform.
       */
      virtual glm::mat4 projection(float aspect, float alpha = 1.0f) const = 0;
  };
} }

#endif
