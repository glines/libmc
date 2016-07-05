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

extern "C" {
#include <mc/algorithms/common/cube.h>
}

#include "../common/glError.h"
#include "../common/shaderProgram.h"
#include "../common/shaders.h"

#include "interpolatingOctree.h"

namespace mc { namespace samples { namespace transition {
  InterpolatingNode::InterpolatingNode(
      InterpolatingNode *parent,
      int index)
    : OctreeNode(parent, index)
  {
  }

  glm::vec3 InterpolatingNode::worldPos() const {
    glm::vec3 result;
    result.x = (float)this->pos().x;
    result.y = (float)this->pos().y;
    result.z = (float)this->pos().z;
    return result;
  }

  float InterpolatingNode::size() const {
    return (float)(1 << this->level());
  }

  void InterpolatingNode::setSample(float value, int sampleIndex) {
    // Iterate over all eight nodes that share this sample
    bool madeRecursiveCall = false;
    int xOffset = (sampleIndex & (1 << 0)) ? 1 : 0;
    int yOffset = (sampleIndex & (1 << 1)) ? 1 : 0;
    int zOffset = (sampleIndex & (1 << 2)) ? 1 : 0;
    for (int z = zOffset ? 0 : -1;
        z <= (zOffset ? 1 : 0);
        ++z)
    {
      for (int y = yOffset ? 0 : -1;
          y <= (yOffset ? 1 : 0);
          ++y)
      {
        for (int x = xOffset ? 0 : -1;
            x <= (xOffset ? 1 : 0);
            ++x)
        {
          OctreeCoordinates relativeOffset;
          relativeOffset.x = x;
          relativeOffset.y = y;
          relativeOffset.z = z;
          // NOTE: When the offset is (0, 0, 0), we set the sample value for
          // this node.
#ifndef NDEBUG
          if (x == y == z == 0) {
            assert(this->hasRelativeNode(relativeOffset));
          }
#endif
          if (this->hasRelativeNode(relativeOffset)) {
            auto node = this->getRelativeNode(relativeOffset);
            int i = 0;
            i |= (relativeOffset.x == 0) ? (xOffset << 0) : ((~xOffset & 1) << 0);
            i |= (relativeOffset.y == 0) ? (yOffset << 1) : ((~yOffset & 1) << 1);
            i |= (relativeOffset.z == 0) ? (zOffset << 2) : ((~zOffset & 1) << 2);
            node->m_samples[i] = value;
#ifndef NDEBUG
            if (x == y == z == 0) {
              assert(node.get() == this);
            }
            if (node.get() == this) {
              assert(x == y == z == 0);
              assert(i == sampleIndex);
            }
#endif
            if (node->hasChild(i) && !madeRecursiveCall) {
              // We recurse if we find any relative nodes at a lower level in
              // the octree, but we only recurse once because making more than
              // one recursion would be redundant.
              auto child = node->getChild(i);
              // Recurse to propagate the sample value to child nodes
              child->setSample(value, i);
              madeRecursiveCall = true;
            }
          }
        }
      }
    }
    assert(this->m_samples[sampleIndex] == value);
  }

  InterpolatingOctree::InterpolatingOctree(
      const glm::vec3 &position,
      const glm::quat &orientation,
      float defaultSample)
    : SceneObject(position, orientation), m_defaultSample(defaultSample)
  {
    // Upload debugging vertices to the GL
    m_generateCubeWireframe();
  }

  float InterpolatingOctree::operator()(float x, float y, float z) {
    // TODO: Look for the octree node that contains this point
    // TODO: Determine if this node needs to be further subdivided before
    // interpolation
    // TODO: Perform trilinear interpolation
  }

  void InterpolatingOctree::setSample(
      const OctreeCoordinates &pos, float value)
  {
    // TODO: We need to set several samples in the octree
    // TODO: Find the octree node at the given position
    auto node = this->getNode(pos);
    node->setSample(value);
  }

  void InterpolatingOctree::m_generateCubeWireframe() {
    // Send a simple wireframe representation of a cube to the GL so that we
    // can draw the octree.
    // Iterate over the cube vertices
    WireframeVertex vertices[8];
    unsigned int pos[3];
    for (int vertex = 0; vertex < 8; ++vertex) {
      mcCube_vertexRelativePosition(vertex, pos);
      vertices[vertex].pos[0] = pos[0] ? 1.0f : 0.0f;
      vertices[vertex].pos[1] = pos[1] ? 1.0f : 0.0f;
      vertices[vertex].pos[2] = pos[2] ? 1.0f : 0.0f;
      vertices[vertex].color[0] = 0.0f;
      vertices[vertex].color[1] = 0.0f;
      vertices[vertex].color[2] = 1.0f;
    }
    // Send the vertices to the GL
    glGenBuffers(1, &m_cubeWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  /* target */
        sizeof(WireframeVertex) * 8,  /* size */
        vertices,  /* data */
        GL_STATIC_DRAW  /* usage */
        );
    FORCE_ASSERT_GL_ERROR();
    // Iterate over cube edges to make edge lines
    unsigned int indices[MC_CUBE_NUM_EDGES * 2];
    for (int edge = 0; edge < MC_CUBE_NUM_EDGES; ++edge) {
      unsigned int vertices[2];
      mcCube_edgeVertices(edge, vertices);
      indices[edge * 2] = vertices[0];
      indices[edge * 2 + 1] = vertices[1];
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

  void InterpolatingOctree::m_drawOctreeWireframe(
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
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

    // Traverse entire octree and draw a cube for each node
    for (auto node : *this) {
      glm::mat4 mw = modelWorld;
      // Translate the cube wireframe to this node's voxel block position
      mw = glm::translate(mw, node.worldPos());
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

  void InterpolatingOctree::draw(const glm::mat4 &modelWorld,
      const glm::mat4 &worldView, const glm::mat4 &projection,
      float alpha, bool debug)
  {
    // Draw a wireframe for the octree nodes
    m_drawOctreeWireframe(modelWorld, worldView, projection);
  }
} } }
