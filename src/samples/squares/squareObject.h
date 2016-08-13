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

#ifndef MC_SAMPLES_SQUARES_SQUARE_OBJECT_H_
#define MC_SAMPLES_SQUARES_SQUARE_OBJECT_H_

#include "../common/sceneObject.h"

#include <mcxx/contour.h>
#include <mcxx/scalarField.h>

namespace mc { namespace samples { namespace squares {
  class SquareObject : public SceneObject {
    private:
      class SquareScalarField : public mc::ScalarField {
        private:
          int m_square;
          float m_intensity;
        public:
          SquareScalarField(
              int square,
              float intensity = 1.0f);
          float operator()(float x, float y, float z);
      };

      typedef struct {
        float pos[3], color[3];
      } WireframeVertex;

      GLuint m_wireframeVertices, m_wireframeIndices;
      int m_numWireframeIndices;

      void m_initWireframe();

      void m_update();
      void m_updateWireframe(const mc::Contour &contour);

      void m_drawWireframe(
          const glm::mat4 &modelView,
          const glm::mat4 &projection);

    public:
      SquareObject(
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat());

      void draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug);
  };
} } }

#endif
