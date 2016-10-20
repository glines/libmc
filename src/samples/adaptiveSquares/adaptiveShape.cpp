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

#include <glm/gtc/type_ptr.hpp>

#include "../common/glError.h"
#include "../common/sceneObject.h"
#include "../common/shaderProgram.h"
#include "../common/shaders.h"
#include "adaptiveShape.h"

namespace mc { namespace samples { namespace adaptiveSquares {
  float shape(float x, float y) {
    static const float radius = 2.0f;
    return x*x + y*y - radius*radius;
  }

  AdaptiveShape::AdaptiveShape()
    : SceneObject()
  {
    mcQuadNodeCoordinates pos;

    mcQuadNode_initRoot(&m_root);

    /* Grow the root to cover our entire shape */
    pos.coord[0] = -(1<<4);
    pos.coord[1] = -(1<<4);
    mcQuadNode_getNode(
        &m_root,
        &pos,  // coordinates
        5  // level
        );

#define COORD_TO_FLOAT(a) ((float)(a) / (1<<4))

    /* Fill the quadtree with nodes that approximate our shape */
    /* Iterate over the quad tree */
    /* NOTE: As we iterate over the quad tree, we are actually adding children
     * to the node we are at. The iterator handles this very well with a
     * depth-first traversal. */
    mcQuadNodeIterator i;
    for (
        i = mcQuadNode_begin(&m_root);
        i.current != NULL; 
        mcQuadNodeIterator_next(&i))
    {
      mcQuadNode *n = i.current;
      if (n->level == 0)
        continue;
      /* Check if the low-order corner of this node straddles the contour line */
      int negative = shape(
          COORD_TO_FLOAT(n->pos.coord[0]),
          COORD_TO_FLOAT(n->pos.coord[1])) < 0.0f ? 1 : 0;
      int straddle = 0;
      for (int j = 0; j < 3; ++j) {
        int otherNegative;
        switch (j) {
          case 0:
            otherNegative = shape(
                COORD_TO_FLOAT(n->pos.coord[0] + (1 << (n->level - 1))),
                COORD_TO_FLOAT(n->pos.coord[1]))
              < 0.0f ? 1 : 0;
            break;
          case 1:
            otherNegative = shape(
                COORD_TO_FLOAT(n->pos.coord[0]),
                COORD_TO_FLOAT(n->pos.coord[1] + (1 << (n->level - 1))))
              < 0.0f ? 1 : 0;
            break;
          case 2:
            otherNegative = shape(
                COORD_TO_FLOAT(n->pos.coord[0] + (1 << (n->level - 1))),
                COORD_TO_FLOAT(n->pos.coord[1] + (1 << (n->level - 1))))
              < 0.0f ? 1 : 0;
            break;
        }
        if (negative != otherNegative) {
          straddle = 1;
          break;
        }
      }
    }
  }

  void AdaptiveShape::m_drawNodes(
      const glm::mat4 &modelView,
      const glm::mat4 &projection)
  {
    // Use the wireframe shader
    auto shader = Shaders::wireframeShader();
    shader->use();

    // Prepare the uniform values
    assert(shader->modelViewLocation() != -1);
    assert(shader->projectionLocation() != -1);
    glUniformMatrix4fv(
        shader->projectionLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(projection)  // value
        );
    ASSERT_GL_ERROR();

    // Prepare the vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, m_squareVertexBuffer);
    ASSERT_GL_ERROR();
    assert(shader->vertPositionLocation() != -1);
    glEnableVertexAttribArray(shader->vertPositionLocation());
    ASSERT_GL_ERROR();
    glVertexAttribPointer(
        shader->vertPositionLocation(),  // index
        3,  // size
        GL_FLOAT,  // type
        0,  // normalized
        sizeof(WireframeVertex),  // stride
        &(((WireframeVertex *)0)->pos[0])  // pointer
        );
    ASSERT_GL_ERROR();
    assert(shader->vertColorLocation() != -1);
    glEnableVertexAttribArray(shader->vertColorLocation());
    ASSERT_GL_ERROR();
    glVertexAttribPointer(
        shader->vertColorLocation(),  // index
        3,  // size
        GL_FLOAT,  // type
        0,  // normalized
        sizeof(WireframeVertex),  // stride
        &(((WireframeVertex *)0)->color[0])  // pointer
        );
    ASSERT_GL_ERROR();

    // Traverse the quadtree and draw a square for each node
    for (auto node : *this) {
      glm::mat4 localModel;
      // Translate the square wireframe to this node's position
      localModel = glm::translate(localModel,
          glm::vec3(node.worldSpacePos()));
      // Scale the square according to this node's quadtree level
      localModel = glm::scale(localModel, glm::vec3(node.size()));
      // Update the model-view transform uniform
      glm::mat4 localView = modelView * localModel;
      glUniformMatrix4fv(
          shader->modelViewLocation(),  // location
          1,  // count
          0,  // transpose
          glm::value_ptr(localView)  // value
          );
      ASSERT_GL_ERROR();
      // Draw the cube wireframe lines
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_squareIndexBuffer);
      ASSERT_GL_ERROR();
      glLineWidth(1.0f);
      ASSERT_GL_ERROR();
      glDrawElements(
          GL_LINES,  // mode
          4 * 2,  // count
          GL_UNSIGNED_INT,  // type
          0  // indices
          );
      ASSERT_GL_ERROR();
    }
  }

  void AdaptiveShape::draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug)
  {
    glm::mat4 modelView = worldView * modelWorld;
    m_drawNodes(modelView, projection);
  }
} } }
