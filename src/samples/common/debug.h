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

#include <GL/glew.h>
#include <memory>
#include <vector>

#include "sceneObject.h"

namespace mc { namespace samples {
  class Debug : public SceneObject {
    private:
      typedef struct {
        float pos[3], color[3];
      } LineVertex;
      typedef struct {
        LineVertex vertices[2];
      } Line;
      typedef struct {
        float pos[3], color[3];
      } Point;
      std::vector<Line> m_lines;
      std::vector<Point> m_points;
      bool m_linesChanged;

      GLuint m_lineBuffer, m_pointBuffer;

      Debug();

      void m_updateLines();
      void m_updatePoints();
      void m_drawLines(
          const glm::mat4 &modelView,
          const glm::mat4 &projection) const;
      void m_drawPoints(
          const glm::mat4 &modelView,
          const glm::mat4 &projection) const;

      void m_drawLine(
          const glm::vec3 &a,
          const glm::vec3 &b,
          const glm::vec3 &color);
      void m_drawPoint(
          const glm::vec3 &pos,
          const glm::vec3 &color);
    public:
      static std::shared_ptr<Debug> instance();

      static void drawLine(
          const glm::vec3 &a,
          const glm::vec3 &b,
          const glm::vec3 &color)
      {
        instance()->m_drawLine(a, b, color);
      }

      static void drawPoint(
          const glm::vec3 &pos,
          const glm::vec3 &color)
      {
        instance()->m_drawPoint(pos, color);
      }

      void draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug);
  };
} }
