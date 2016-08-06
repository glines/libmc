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

#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C" {
#include <mc/algorithms/common/cube.h>
}

#include "../common/glError.h"
#include "../common/perspectiveCamera.h"
#include "../common/shaderProgram.h"
#include "../common/shaders.h"
#include "frustum.h"

#include "transvoxelTerrain.h"

namespace mc { namespace samples { namespace transvoxel {
  TransvoxelTerrain::TransvoxelTerrain(
      std::shared_ptr<PerspectiveCamera> camera, float aspect, int minimumLod)
    : m_camera(camera), m_minimumLod(minimumLod), m_aspect(aspect)
  {
    /* Send buffers to the GL for drawing the octree wireframe */
    m_generateCubeWireframe();
    /* Send buffers to the GL for drawing the view frustum wireframe */
    m_generateFrustumWireframe();
  }

  bool TransvoxelTerrain::handleEvent(const SDL_Event &event) {
    switch (event.type) {
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            /* Update the aspect ratio now that the window size changed */
            auto width = event.window.data1;
            auto height = event.window.data2;
            m_aspect = (float)width / (float)height;
            /* TODO: Update the frustum object? */
            /* We read window size events but do not consume the event */
            break;
        }
        break;
    }
    return false;
  }

  void TransvoxelTerrain::tick(float dt) {
    /* TODO: Check if the camera moved? */

    /* TODO: Check the camera's view frustum against the octree */
    m_updateVisibility();
  }

  void TransvoxelTerrain::m_updateVisibility() {
    static bool once = false;  // XXX
    if (once) {
      return;
    } else {
      once = true;
    }
    assert(m_aspect != 0.0f);
    /* The view frustum can be expressed as the world->view->projection
     * transformation matrix */
    Frustum frustum(*m_camera, m_aspect);
    /* We need to grow the octree if the root is not large enough */
    m_ensureRootContainsFrustum(frustum);
    /* TODO: Traverse the octree from top to bottom checking each octnode for
     * visibility */
    auto root = m_tree.root();
    m_updateNodeVisibility(frustum, root);
    glm::vec4 test = m_camera->projection(m_aspect) * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f); // XXX
    fprintf(stderr,
        "right plane normal test: (%g, %g, %g, %g)\n",
        test.x,
        test.y,
        test.z,
        test.w);
  }

  void TransvoxelTerrain::m_ensureRootContainsFrustum(
      const Frustum &frustum)
  {
    while (!m_checkRootContainsFrustum(frustum)) {
      fprintf(stderr, "Growing the octree root\n");
      m_tree.growRoot();
    }
  }

  bool TransvoxelTerrain::m_checkRootContainsFrustum(const Frustum &frustum) {
    /* Check if all frustum vertices are contained within the root node */
    for (int i = 0; i < 8; ++i) {
      auto pos = frustum.vertexPosition(i);
      /*
      fprintf(stderr, "frustum vertex '%d' pos: (%g, %g, %g)\n",
          i,
          pos.x,
          pos.y,
          pos.z);
          */
      if (!m_tree.root()->contains(pos)) {
        fprintf(stderr, "The root DOES NOT contain the frustum!\n");
        return false;
      }
    }
//    fprintf(stderr, "The root contains the frustum\n");
    return true;
  }

  bool TransvoxelTerrain::m_updateNodeVisibility(
    const Frustum &frustum, std::shared_ptr<TransvoxelNode> node)
  {
    bool oldVisibility = node->isVisible();
    bool newVisibility = frustum.testVisibility(*node);
    if (!newVisibility) {
      node->setVisible(false);
      return false;
    }
    /* TODO: Stop recursing once the node's size projected on the near plane is
     * below some threshold. */
    static const float threshold = 0.05f;
//    if (node->projectedSize(frustum) < threshold) {}
    if (node->level() == 0)  // XXX
      return newVisibility;
    for (int i = 0; i < 8; ++i) {
      auto child = node->getChild(i);
      fprintf(stderr,
          "node->level(): %d\n"
          "child->level(): %d\n",
          node->level(),
          child->level());
      assert(child->level() < node->level());
      if (!child)
        break;
      m_updateNodeVisibility(frustum, child);
    }
    return newVisibility;
  }

  void TransvoxelTerrain::m_generateCubeWireframe() {
    // Send a simple wireframe representation of a cube to the GL so that we
    // can draw the octree.
    // Iterate over the cube vertices
    WireframeVertex vertices[8];
    unsigned int pos[3];
    for (int sampleIndex = 0; sampleIndex < 8; ++sampleIndex) {
      mcCube_sampleRelativePosition(sampleIndex, pos);
      vertices[sampleIndex].pos[0] = pos[0] ? 1.0f : 0.0f;
      vertices[sampleIndex].pos[1] = pos[1] ? 1.0f : 0.0f;
      vertices[sampleIndex].pos[2] = pos[2] ? 1.0f : 0.0f;
      vertices[sampleIndex].color[0] = 0.0f;
      vertices[sampleIndex].color[1] = 0.0f;
      vertices[sampleIndex].color[2] = 1.0f;
    }
    // Send the vertices to the GL
    glGenBuffers(1, &m_cubeWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  /* target */
        sizeof(vertices),  /* size */
        vertices,  /* data */
        GL_STATIC_DRAW  /* usage */
        );
    FORCE_ASSERT_GL_ERROR();
    // Iterate over cube edges to make edge lines
    unsigned int indices[MC_CUBE_NUM_EDGES * 2];
    for (int edge = 0; edge < MC_CUBE_NUM_EDGES; ++edge) {
      unsigned int sampleIndices[2];
      mcCube_edgeSampleIndices(edge, sampleIndices);
      indices[edge * 2] = sampleIndices[0];
      indices[edge * 2 + 1] = sampleIndices[1];
    }
    // Send the indices to the GL
    glGenBuffers(1, &m_cubeWireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeWireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  /* target */
        sizeof(unsigned int) * MC_CUBE_NUM_EDGES * 2,  /* size */
        indices,  /* data */
        GL_STATIC_DRAW  /* usage */
        );
    FORCE_ASSERT_GL_ERROR();
  }

  void TransvoxelTerrain::m_generateFrustumWireframe() {
    // Set the indices needed for drawing. Note that vertex indices are given
    // in bitwise order, with x, y, and z being represented by the 1st, 2nd,
    // and 3rd bits respectively.
    unsigned int indices[MC_CUBE_NUM_EDGES * 2];
    for (int edge = 0; edge < MC_CUBE_NUM_EDGES; ++edge) {
      unsigned int sampleIndices[2];
      mcCube_edgeSampleIndices(edge, sampleIndices);
      indices[edge * 2] = sampleIndices[0];
      indices[edge * 2 + 1] = sampleIndices[1];
    }
    // Send the indices to the GL
    glGenBuffers(1, &m_frustumWireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_frustumWireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  /* target */
        sizeof(unsigned int) * MC_CUBE_NUM_EDGES * 2,  /* size */
        indices,  /* data */
        GL_STATIC_DRAW  /* usage */
        );
    FORCE_ASSERT_GL_ERROR();
    // Update the view frustum vertex positions
    glGenBuffers(1, &m_frustumWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    m_updateFrustumWireframe();
  }

  void TransvoxelTerrain::m_updateFrustumWireframe() {
    // Find each frustum vertex and put it in a buffer
    WireframeVertex vertices[8];
    Frustum frustum(*m_camera, m_aspect);
    for (int i = 0; i < 8; ++i) {
      auto pos = frustum.vertexPosition(i);
      vertices[i].pos[0] = pos.x;
      vertices[i].pos[1] = pos.y;
      vertices[i].pos[2] = pos.z;
      vertices[i].color[0] = 1.0f;
      vertices[i].color[1] = 1.0f;
      vertices[i].color[2] = 1.0f;
    }
    // Send the buffer to the GL
    glBindBuffer(GL_ARRAY_BUFFER, m_frustumWireframeVertices);
    ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  // target
        sizeof(vertices),  // size
        vertices,  // data
        GL_DYNAMIC_DRAW  // usage
        );
  }

  void TransvoxelTerrain::m_drawOctree(
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
      const glm::mat4 &projection)
  {
    // Use our wireframe drawing shader
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
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeWireframeVertices);
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
    // Traverse the LOD octree and draw a cube for each node
    for (auto node : m_tree) {
      glm::mat4 mw = modelWorld;
      // Translate the cube wireframe to this node's voxel block position
      mw = glm::translate(mw, node.worldSpacePos());
      // Scale the cube wireframe according to this node's level of detail
      mw = glm::scale(mw, glm::vec3(node.size()));
      // Update the model-view transform uniform
      glm::mat4 mv = worldView * mw;
      glUniformMatrix4fv(
          shader->modelViewLocation(),  // location
          1,  // count
          0,  // transpose
          glm::value_ptr(mv)  // value
          );
      ASSERT_GL_ERROR();
      // Draw the cube wireframe lines
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeWireframeIndices);
      ASSERT_GL_ERROR();
      glLineWidth(1.0f);
      ASSERT_GL_ERROR();
      glDrawElements(
          GL_LINES,  // mode
          MC_CUBE_NUM_EDGES * 2,  // count
          GL_UNSIGNED_INT,  // type
          0  // indices
          );
      ASSERT_GL_ERROR();
    }
  }

  void TransvoxelTerrain::m_drawViewFrustum(
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
      const glm::mat4 &projection)
  {
    glm::mat4 modelView = worldView * modelWorld;
    // Update the view frustum wireframe
    m_updateFrustumWireframe();
    // Use our wireframe drawing shader
    auto shader = Shaders::wireframeShader();
    shader->use();
    // Prepare the uniform values
    assert(shader->modelViewLocation() != -1);
    glUniformMatrix4fv(
        shader->modelViewLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(modelView)  // value
        );
    assert(shader->projectionLocation() != -1);
    glUniformMatrix4fv(
        shader->projectionLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(projection)  // value
        );
    ASSERT_GL_ERROR();

    // Prepare the vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, m_frustumWireframeVertices);
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
    // Draw the frustum wireframe lines
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_frustumWireframeIndices);
    ASSERT_GL_ERROR();
    glLineWidth(1.0f);
    ASSERT_GL_ERROR();
    glDrawElements(
        GL_LINES,  // mode
        MC_CUBE_NUM_EDGES * 2,  // count
        GL_UNSIGNED_INT,  // type
        0  // indices
        );
    ASSERT_GL_ERROR();
  }

  void TransvoxelTerrain::draw(const glm::mat4 &modelWorld,
      const glm::mat4 &worldView, const glm::mat4 &projection,
      float alpha, bool debug)
  {
    // Draw a wireframe representation of the octree
    m_drawOctree(modelWorld, worldView, projection);
    // Draw a wireframe representation of the camera frustum
    m_drawViewFrustum(modelWorld, worldView, projection);
  }
} } }
