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

#include <cstdlib>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/glError.h"
#include "../common/shaderProgram.h"
#include "../common/shaders.h"
#include "coloredQuadTree.h"

namespace mc { namespace samples { namespace cascading {
  void fillQuadTree(mcQuadNode *root) {
    mcQuadNodeCoordinates pos;
    for (int level = 4; level >= 0; level--) {
      for (int y = 0; y < 30; ++y) {
        for (int x = 0; x < 30; ++x) {
          if (rand() % 10 != 0)
            continue;
          pos.coord[0] = x - 15;
          pos.coord[1] = y - 15;
          mcQuadNode *node = mcQuadNode_getNode(root, &pos, level);
        }
      }
    }
  }

  void ColoredQuadTree::m_addPoints() {
    ColoredPoint point;
    point.color = 0;
    for (int y = 0; y < 10; ++y) {
      for (int x = 0; x < 10; ++x) {
        if (rand() % 10 == 0)
          continue;
        point.pos = glm::vec2(
            (float)x + (float)(rand() % 40 - 20),
            (float)y + (float)(rand() % 40 - 20));
        m_points.push_back(point);
      }
    }
  }

  ColoredQuadTree::ColoredQuadTree(std::shared_ptr<Camera> camera)
    : m_camera(camera)  /* NOTE: The camera is needed to project mouse clicks
                           into world space */
  {
    mcQuadNode_initRoot(&m_root);
    /* Fill the quad tree with sample data */
    fillQuadTree(&m_root);

    /* Send buffers to the GL */
    m_generateSquare();
    m_generateBillboard();

    /* Add some test points */
    m_addPoints();
  }

  void ColoredQuadTree::m_generateSquare() {
    /* Send square vertices to the GL */
    WireframeVertex vertices[4] {
      { .pos = { 0.0f, 0.0f, 0.0f },
        .color = { 0.0f, 0.0f, 1.0f },
      },
      { .pos = { 1.0f, 0.0f, 0.0f },
        .color = { 0.0f, 0.0f, 1.0f },
      },
      { .pos = { 0.0f, 1.0f, 0.0f },
        .color = { 0.0f, 0.0f, 1.0f },
      },
      { .pos = { 1.0f, 1.0f, 0.0f },
        .color = { 0.0f, 0.0f, 1.0f },
      },
    };
    glGenBuffers(1, &m_squareVertices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_squareVertices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  // target
        sizeof(vertices),  // size
        vertices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
    /* Send square indices to the GL */
    unsigned int indices[] = {
      0, 1,  // bottom
      1, 3,  // right
      3, 2,  // top
      2, 0,  // left
    };
    glGenBuffers(1, &m_squareIndices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_squareIndices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  // target
        sizeof(indices),  // size
        indices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
  }

  void ColoredQuadTree::m_generateBillboard() {
    // Make four vertices in a quad arrangement around a center point
    BillboardVertex vertices[] = {
      { .pos = { -1.0f, -1.0f, -5.0f, }, .tex = { 0.0f, 0.0f, }, },
      { .pos = {  1.0f, -1.0f, -5.0f, }, .tex = { 1.0f, 0.0f, }, },
      { .pos = { -1.0f,  1.0f, -5.0f, }, .tex = { 0.0f, 1.0f, }, },
      { .pos = {  1.0f,  1.0f, -5.0f, }, .tex = { 1.0f, 1.0f, }, },
    };
    // Send the vertex data to the GL
    glGenBuffers(1, &m_billboardVertices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_billboardVertices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  // target
        sizeof(vertices),  // size
        vertices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
    // Make two triangles into a quad from our vertices
    unsigned int indices[] = {
      0, 1, 3,
      3, 2, 0,
    };
    // Send the indices to the GL
    glGenBuffers(1, &m_billboardIndices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_billboardIndices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  // target
        sizeof(indices),  // size
        indices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
  }

  void ColoredQuadTree::m_drawPoint(
      const glm::vec3 &pos,
      const glm::vec3 &color,
      std::shared_ptr<ShaderProgram> shader,
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
      const glm::mat4 &projection) const
  {
    // We can assume that the billboard point shader is already set up; we
    // just need to make the draw call for the given sample point
    glm::mat4 localModel;
    // Translate the point to its position
    localModel = glm::translate(localModel, pos);
    // Remove the rotation component of the matrix for proper billboarding
    glm::mat4 localView = worldView * modelWorld * localModel;
    localView[0][0] = 1.0f;
    localView[0][1] = 0.0f;
    localView[0][2] = 0.0f;
    localView[1][0] = 0.0f;
    localView[1][1] = 1.0f;
    localView[1][2] = 0.0f;
    localView[2][0] = 0.0f;
    localView[2][1] = 0.0f;
    localView[2][2] = 1.0f;
//    localView = glm::scale(localView, glm::vec3(0.16));  // XXX
    // Update the model-view transform uniform
    glUniformMatrix4fv(
        shader->modelViewLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(localView)  // value
        );
    ASSERT_GL_ERROR();
    // Update the color
    glUniform3f(
        shader->colorLocation(),  // location
        color.x,  // v0
        color.y,  // v1
        color.z  // v2
        );
    ASSERT_GL_ERROR();
    // Draw the billboarded points
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_billboardIndices);
    ASSERT_GL_ERROR();
    glDrawElements(
        GL_TRIANGLES,  // mode
        6,  // count
        GL_UNSIGNED_INT,  // type
        0  // indices
        );
    ASSERT_GL_ERROR();
  }

  void ColoredQuadTree::m_drawPoints(
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
      const glm::mat4 &projection) const
  {
    static const glm::vec3 colors[] = {
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 1.0f),
    };

    // Use the billboard point shader
    auto shader = Shaders::billboardPointShader();
    shader->use();

    // Enable alpha blending
    glEnable(GL_BLEND);
    ASSERT_GL_ERROR();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ASSERT_GL_ERROR();

    // Prepare the uniform values
    assert(shader->modelViewLocation() != -1);
    assert(shader->colorLocation() != -1);
    assert(shader->projectionLocation() != -1);
    glUniformMatrix4fv(
        shader->projectionLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(projection)  // value
        );
    ASSERT_GL_ERROR();

    // Prepare the vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, m_billboardVertices);
    ASSERT_GL_ERROR();
    assert(shader->vertPositionLocation() != -1);
    glEnableVertexAttribArray(shader->vertPositionLocation());
    ASSERT_GL_ERROR();
    glVertexAttribPointer(
        shader->vertPositionLocation(),  // index
        3,  // size
        GL_FLOAT,  // type
        0,  // normalized
        sizeof(BillboardVertex),  // stride
        &(((BillboardVertex *)0)->pos[0])  // pointer
        );
    assert(shader->vertTexCoordLocation() != -1);
    glEnableVertexAttribArray(shader->vertTexCoordLocation());
    ASSERT_GL_ERROR();
    glVertexAttribPointer(
        shader->vertTexCoordLocation(),  // index
        2,  // size
        GL_FLOAT,  // type
        0,  // normalized
        sizeof(BillboardVertex),  // stride
        &(((BillboardVertex *)0)->tex[0])  // pointer
        );
    ASSERT_GL_ERROR();
    // Iterate over all colored points
    for (auto point : m_points) {
      // Draw this point
      m_drawPoint(
          glm::vec3(point.pos, -8.0f),
          colors[point.color],
          shader,
          modelWorld,
          worldView,
          projection);
    }
  }

  void ColoredQuadTree::m_drawNodes(
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
    glBindBuffer(GL_ARRAY_BUFFER, m_squareVertices);
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

    // Traverse the quad tree and draw a square for each node
    for (auto i = mcQuadNode_begin(&m_root);
        !mcQuadNodeIterator_equals(
          &i, mcQuadNode_end(&m_root));
        mcQuadNodeIterator_next(&i))
    {
      fprintf(stderr,
          "current: 0x%p\n"
          "  level: %d\n"
          "  pos: { %d, %d }\n",
          i.current,
          i.current->level,
          i.current->pos.coord[0],
          i.current->pos.coord[1]);
      glm::mat4 localModel;
      // Translate the square wireframe to this node's position
      localModel = glm::translate(localModel,
          glm::vec3(
            (float)i.current->pos.coord[0],
            (float)i.current->pos.coord[1],
            0.0f));
      // Scale the square according to this node's quadtree level
      localModel = glm::scale(localModel,
          glm::vec3((float)(1 << i.current->level)));
      // Update the model-view transform uniform
      glm::mat4 localView = modelView * localModel;
      glUniformMatrix4fv(
          shader->modelViewLocation(),  // location
          1,  // count
          0,  // transpose
          glm::value_ptr(localView)  // value
          );
      ASSERT_GL_ERROR();
      // Draw the square wireframe lines
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_squareIndices);
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

  void ColoredQuadTree::draw(const glm::mat4 &modelWorld,
      const glm::mat4 &worldView, const glm::mat4 &projection,
      float alpha, bool debug)
  {
    glm::mat4 modelView = worldView * modelWorld;
    m_drawNodes(modelView, projection);
    m_drawPoints(modelWorld, worldView, projection);
  }
} } }
