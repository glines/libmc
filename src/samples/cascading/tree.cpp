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

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../common/camera.h"
#include "../common/glError.h"
#include "../common/shaderProgram.h"
#include "../common/shaders.h"

#include "tree.h"

#define min(a, b) ((a) < (b) ? (a) : (b))

namespace mc { namespace samples { namespace cascading {
  TreeNode::TreeNode(TreeNode *parent, int index)
    : QuadtreeNode<TreeNode>(parent, index)
  {
    for (int i = 0; i < 4; ++i) {
      m_points[i] = 1.0f;
    }
  }

  bool TreeNode::m_containsPoint(const QuadtreeCoordinates &point) {
#define CHECK_AXIS_CONTAINS_POINT(axis) \
    if (point.axis < this->pos().axis) \
      return false; \
    if (point.axis > this->pos().axis + (1 << this->level())) \
      return false;
    CHECK_AXIS_CONTAINS_POINT(x)
    CHECK_AXIS_CONTAINS_POINT(y)
    return true;
  }

  bool TreeNode::m_hasPoint(const QuadtreeCoordinates &point) {
#define CHECK_AXIS_HAS_POINT(axis) \
    if (point.axis != this->pos().axis \
        && point.axis != this->pos().axis + (1 << this->level())) \
    { return false; }
    CHECK_AXIS_HAS_POINT(x)
    CHECK_AXIS_HAS_POINT(y)
    return true;
  }

  void TreeNode::m_setPoint(
      const QuadtreeCoordinates &point,
      float value)
  {
    assert(this->m_containsPoint(point));
    if (this->m_hasPoint(point)) {
      /* Calculate the point index */
      int pointIndex = 0;
      pointIndex |= point.x == this->pos().x ? 0 : 1 << 0;
      pointIndex |= point.y == this->pos().y ? 0 : 1 << 1;
      /* Set the point value */
      m_points[pointIndex] = value;
      fprintf(stderr,
          "We set the point to value: %g\n",
          value);
    }
    for (int i = 0; i < 4; ++i) {
      auto child = this->child(i);
      if (!child) {
        continue;
      }
      if (child->m_containsPoint(point)) {
        /* Recursively set all children containing this point*/
        child->m_setPoint(point, value);
      }
    }
  }

  glm::vec3 TreeNode::worldSpacePos() const {
    glm::vec3 result;
    result.x = (float)this->pos().x;
    result.y = (float)this->pos().y;
    result.z = 0.0f;
    return result;
  }

  float TreeNode::size() const {
    /* Compute the world space size of this node */
    return (float)(1 << this->level());
  }

  bool Tree::m_intersectRay(const Ray &r, QuadtreeCoordinates *result) {
    /* Intersect the ray and the quadtree plane in world space */
    Plane p;
    p.p = this->position();
    p.n = glm::mat3_cast(this->orientation()) * glm::vec3(0.0f, 0.0f, 1.0f);
    float denominator = glm::dot(r.v, p.n);
    if (denominator == 0.0f) {
      return false;
    }
    float numerator = glm::dot(p.p - r.p, p.n);
    float t = numerator / denominator;
    glm::vec3 intersection = r.p + t * r.v;
    /* Determine the nearest quadtree coordinates from the calculated
     * intersection intersection */
    /* NOTE: We assume here that the quadtree is on the xy-plane */
    result->x = (int)floor(intersection.x);
    result->y = (int)floor(intersection.y);
    fprintf(stderr, "intersection result: (%d, %d)\n",
        result->x,
        result->y);
  }

  void Tree::m_setPoint(const QuadtreeCoordinates &pos, float value) {
    /* Calculate the highest level that aligns with this position */
    int level;
    for (level = 0; level < sizeof(int) * 8 - 1; ++level) {
#define CHECK_AXIS_ALIGNED(axis) \
      if (pos.axis & (1 << level)) break;
      CHECK_AXIS_ALIGNED(x)
      CHECK_AXIS_ALIGNED(y)
    }
    fprintf(stderr,
        "pos: (0x%02x, 0x%02x)\n"
        "  level: %d\n",
        pos.x, pos.y, level);
    /* Make sure the user did not click outside of the root */
    if (!this->root()->m_containsPoint(pos)) {
      fprintf(stderr, "root does not contain this point!\n");
      return;
    }
    this->root()->m_setPoint(pos, value);
  }

  Tree::Tree(std::shared_ptr<Camera> camera)
    : m_camera(camera)
  {
    // Send our buffers to the GL
    m_generateSquare();
    m_generateBillboard();
  }

  bool Tree::handleEvent(const SDL_Event &event) {
    switch (event.type) {
      case SDL_MOUSEBUTTONDOWN:
        /* TODO: Calculate the position of the mouse click in the quadtree
         * uniform grid */
        /* TODO: Project a ray from the camera through the quadtree */
        auto window = SDL_GetWindowFromID(event.button.windowID);
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        /* Normalize the mouse click coordinates */
        float x_norm, y_norm;
        x_norm = ((float)event.button.x / (float)w) * 2.0f - 1.0f;
        y_norm = -(((float)event.button.y / (float)h) * 2.0f - 1.0f);
        fprintf(stderr,
            "x_norm: %g\n"
            "y_norm: %g\n",
            x_norm, y_norm);
        /* Project the mouse click as a ray in world space */
        float aspect = (float)w / (float)h;
        Ray r;
        r.p = glm::vec3(
            glm::inverse(m_camera->projection(aspect) * m_camera->worldView())
          * glm::vec4(x_norm, y_norm, -1.0f, 1.0f));
        fprintf(stderr,
            "r.p: (%g, %g, %g)\n",
            r.p.x, r.p.y, r.p.z);
        r.v = glm::vec3(0.0f, 0.0f, -1.0f);
        QuadtreeCoordinates pos;
        pos.x = (int)floor(r.p.x);
        pos.y = (int)floor(r.p.y);
        fprintf(stderr,
            "pos: (%d, %d)\n",
            pos.x,
            pos.y);
        switch (event.button.button) {
          case SDL_BUTTON_RIGHT:
            {
              auto node = this->getNode(pos, 0);
              fprintf(stderr,
                  "node world pos: (%g, %g, %g)\n",
                  node->worldSpacePos().x,
                  node->worldSpacePos().y,
                  node->worldSpacePos().z);
            }
            break;
          case SDL_BUTTON_LEFT:
            m_setPoint(pos, -1.0f);
            break;
        }
//        m_intersectRay(r, &result);
        break;
    }
  }

  void Tree::m_generateSquare() {
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
    glGenBuffers(1, &m_squareVertexBuffer);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_squareVertexBuffer);
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
    glGenBuffers(1, &m_squareIndexBuffer);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_squareIndexBuffer);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  // target
        sizeof(indices),  // size
        indices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
  }

  void Tree::m_generateBillboard() {
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

  void Tree::m_drawNodes(
      const glm::mat4 &modelView,
      const glm::mat4 &projection) const
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

  void Tree::m_drawNodePoint(
      const QuadtreeCoordinates &point,
      float value,
      std::shared_ptr<ShaderProgram> shader,
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
      const glm::mat4 &projection) const
  {
    glm::vec3 pos((float)point.x, (float)point.y, -2.0f);
    // We can assume that the billboard point shader is already set up; we just
    // need to make the draw call for the given sample point
    glm::mat4 mw = modelWorld;
    // Translate the sample to its position
    mw = glm::translate(mw, pos);
    // Remove the rotation component of the matrix for proper billboarding
    glm::mat4 mv = worldView * mw;
    mv[0][0] = 1.0f;
    mv[0][1] = 0.0f;
    mv[0][2] = 0.0f;
    mv[1][0] = 0.0f;
    mv[1][1] = 1.0f;
    mv[1][2] = 0.0f;
    mv[2][0] = 0.0f;
    mv[2][1] = 0.0f;
    mv[2][2] = 1.0f;
    mv = glm::scale(mv, glm::vec3(0.16));  // XXX
    // Update the model-view transform uniform
    glUniformMatrix4fv(
        shader->modelViewLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(mv)  // value
        );
    ASSERT_GL_ERROR();
    // Set the color based on the sample value
    float color[3];
    if (value > 0) {
      color[0] = 1.0f;
      color[1] = 0.0f;
      color[2] = 0.0f;
    } else {
      color[0] = 0.0f;
      color[1] = 1.0f;
      color[2] = 0.0f;
    }
    glUniform3f(
        shader->colorLocation(),  // location
        color[0],  // v0
        color[1],  // v1
        color[2]  // v2
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

  void Tree::m_drawNodePoints(
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
      const glm::mat4 &projection) const
  {
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

#define DRAW_POINT(point) \
    m_drawNodePoint( \
        point, \
        value, \
        shader, \
        modelWorld, \
        worldView, \
        projection)
    // Iterate over all quadtree nodes
    for (auto node : *this) {
      // Iterate over all node points
      for (int i = 0; i < 4; ++i) {
        float value = node.point(i);
        QuadtreeCoordinates point;
#define CALC_POINT_FOR_AXIS(axis, axisNum) \
        point.axis = i & (1 << axisNum) ? \
          node.pos().axis + (1 << node.level()) \
          : node.pos().axis;
        CALC_POINT_FOR_AXIS(x, 0)
        CALC_POINT_FOR_AXIS(y, 1)
        // TODO: Draw this node point
        DRAW_POINT(point);
      }
    }
  }

  void Tree::draw(const glm::mat4 &modelWorld,
      const glm::mat4 &worldView, const glm::mat4 &projection,
      float alpha, bool debug)
  {
    glm::mat4 modelView = worldView * modelWorld;

    m_drawNodePoints(
        modelWorld,
        worldView,
        projection);
    m_drawNodes(modelView, projection);
  }
} } }
