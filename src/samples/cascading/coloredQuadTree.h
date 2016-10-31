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
#include <vector>

extern "C" {
#include <mc/common/quadNode.h>
}

#include "../common/sceneObject.h"

namespace mc { namespace samples {
  class Camera;
  namespace cascading {
    class ColoredQuadTree : public SceneObject {
      private:
        typedef struct {
          float pos[3];
          float color[3];
        } WireframeVertex;
        typedef struct {
          float pos[3];
          float tex[3];
        } BillboardVertex;
        typedef struct {
          glm::vec2 pos;
          int color;
        } ColoredPoint;

        std::shared_ptr<Camera> m_camera;
        GLuint m_squareVertices, m_squareIndices,
               m_billboardVertices, m_billboardIndices;
        mcQuadNode m_root;
        std::vector<ColoredPoint> m_points;

        void m_generateSquare();
        void m_generateBillboard();

        void m_addPoints();
        void m_drawPoint(
            const glm::vec3 &pos,
            const glm::vec3 &color,
            std::shared_ptr<ShaderProgram> shader,
            const glm::mat4 &modelWorld,
            const glm::mat4 &worldView,
            const glm::mat4 &projection) const;
        void m_drawPoints(
            const glm::mat4 &modelWorld,
            const glm::mat4 &worldView,
            const glm::mat4 &projection) const;
        void m_drawNodes(
            const glm::mat4 &modelView,
            const glm::mat4 &projection);
      public:
        ColoredQuadTree(std::shared_ptr<Camera> camera);

        void draw(const glm::mat4 &modelWorld,
            const glm::mat4 &worldView, const glm::mat4 &projection,
            float alpha, bool debug);
    };
  }
} }
