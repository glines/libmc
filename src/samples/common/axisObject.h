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

#ifndef MC_SAMPLES_COMMON_AXIS_OBJECT_H_
#define MC_SAMPLES_COMMON_AXIS_OBJECT_H_

#include "sceneObject.h"

namespace mc { namespace samples {
  /**
   * A debugging object that shows the XYZ-axis as oriented lines with red,
   * green, and blue corresponding to the X, Y, and Z axis respectively.
   */
  class AxisObject : public SceneObject {
    private:
      typedef struct Vertex {
        float pos[3];
        float color[3];
      } Vertex;

      GLuint m_vertexBuffer;

    public:
      /**
       * Construct an AxisObject object. The default parameters place the
       * object at the origin of the scene.
       *
       * \param position Scene position of the axis object.
       * \param orientation Scene orientation of the axis object.
       */
      AxisObject(
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat());

      /**
       * Draw the XYZ-axis. The axis is drawn as oriented lines, with red,
       * green, and blue corresponding to the X, Y, and Z axis respectively.
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
       */
      void draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug);
  };
} }

#endif
