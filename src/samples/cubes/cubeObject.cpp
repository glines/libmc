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

#include <cassert>
#include <glm/gtc/type_ptr.hpp>

extern "C" {
#include <mc/algorithms/simple/common.h>
}

#include "../common/glError.h"
#include "../common/shaderProgram.h"
#include "cubeObject.h"

namespace mc {namespace samples { namespace cubes {
  CubeObject::CubeObject(
      unsigned int cube,
      const glm::vec3 &position,
      const glm::quat &orientation)
    : SceneObject(position, orientation)
  {
    // Generate a simple cube wireframe and send it to the GL
    glGenBuffers(1, &m_cubeWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glGenBuffers(1, &m_cubeWireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    m_generateCubeWireframe();

    // Initialize the cube and send isosurface triangles, etc. to the GL
    glGenBuffers(1, &m_triangleWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glGenBuffers(1, &m_triangleWireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glGenBuffers(1, &m_pointBuffer);
    FORCE_ASSERT_GL_ERROR();
    this->setCube(cube);
  }

  void CubeObject::m_generateCubeWireframe() {
    // Iterate over the cube vertices
    Vertex vertices[8];
    unsigned int pos[3];
    for (int vertex = 0; vertex < 8; ++vertex) {
      mcSimpleVertexRelativePosition(vertex, pos);
      vertices[vertex].pos[0] = pos[0] ? 1.0f : -1.0f;
      vertices[vertex].pos[1] = pos[1] ? 1.0f : -1.0f;
      vertices[vertex].pos[2] = pos[2] ? 1.0f : -1.0f;
      vertices[vertex].color[0] = 0.0f;
      vertices[vertex].color[1] = 0.0f;
      vertices[vertex].color[2] = 1.0f;
    }
    // Send the vertices to the GL
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  /* target */
        sizeof(Vertex) * 8,  /* size */
        vertices,  /* data */
        GL_STATIC_DRAW  /* usage */
        );
    FORCE_ASSERT_GL_ERROR();
    // Iterate over cube edges to make edge lines
    unsigned int indices[MC_SIMPLE_MAX_EDGES * 2];
    for (int edge = 0; edge < MC_SIMPLE_MAX_EDGES; ++edge) {
      unsigned int vertices[2];
      mcSimpleEdgeVertices(edge, vertices);
      indices[edge * 2] = vertices[0];
      indices[edge * 2 + 1] = vertices[1];
    }
    // Send the indices to the GL
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeWireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  /* target */
        sizeof(unsigned int) * MC_SIMPLE_MAX_EDGES * 2,  /* size */
        indices,  /* data */
        GL_STATIC_DRAW  /* usage */
        );
    FORCE_ASSERT_GL_ERROR();
  }

  void CubeObject::m_generateTriangleWireframe(const Mesh *mesh) {
    /* TODO */
  }

  void CubeObject::m_generateDebugPoints(const Mesh *mesh) {
    CubeScalarField sf(m_cube);

    // TODO: Generate points at cube vertices
    // TODO: Generate triangle points at edge intersections

    // Generate a grid of debugging points and send them to the GL
    const int w = 20, h = 20, d = 20;
    Vertex *points = new Vertex[w * h * d];
    for (int z = 0; z < d; ++z) {
      for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
          int i = x + y * w + z * w * h;
          points[i].pos[0] = 2.0f * (float)x / (float)w - 1.0f;
          points[i].pos[1] = 2.0f * (float)y / (float)h - 1.0f;
          points[i].pos[2] = 2.0f * (float)z / (float)d - 1.0f;
          float value = sf(
              (float)x / (float)w,
              (float)y / (float)h,
              (float)z / (float)d);
          if (value >= 0.0f) {
            points[i].color[0] = 0.0f;
            points[i].color[1] = value;
            points[i].color[2] = 0.0f;
          } else {
            points[i].color[0] = -value;
            points[i].color[1] = 0.0f;
            points[i].color[2] = 0.0f;
          }
        }
      }
    }

    // Send the points to the GL
    m_numPoints = w * h * d;
    glBindBuffer(GL_ARRAY_BUFFER, m_pointBuffer);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  // target
        sizeof(Vertex) * m_numPoints,  // size
        points,  // data
        GL_STATIC_DRAW  // usage
        );
  }

  std::shared_ptr<ShaderProgram> CubeObject::m_pointShader() {
    static std::shared_ptr<ShaderProgram> instance =
      std::shared_ptr<ShaderProgram>(
          new ShaderProgram(
#ifdef __EMSCRIPTEN__
            "./assets/shaders/webgl/point.vert",
            "./assets/shaders/webgl/point.frag"
#else
            "./assets/shaders/glsl/point.vert",
            "./assets/shaders/glsl/point.frag"
#endif
            ));
    return instance;
  }

  void CubeObject::m_drawCubeWireframe(
      const glm::mat4 &modelView,
      const glm::mat4 &projection) const
  {
    /* TODO */
  }

  void CubeObject::m_drawTriangleWireframe(
      const glm::mat4 &modelView,
      const glm::mat4 &projection) const
  {
    /* TODO */
  }

  void CubeObject::m_drawDebugPoints(
      const glm::mat4 &modelView,
      const glm::mat4 &projection) const
  {
    // Use our shader for drawing points
    std::shared_ptr<ShaderProgram> shader = m_pointShader();
    shader->use();

    // Prepare the uniform values
    assert(shader->modelViewLocation() != -1);
    glUniformMatrix4fv(
        shader->modelViewLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(modelView)  // value
        );
    ASSERT_GL_ERROR();
    assert(shader->projectionLocation() != -1);
    glUniformMatrix4fv(
        shader->projectionLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(projection)  // value
        );
    ASSERT_GL_ERROR();

    // Prepare the vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, m_pointBuffer);
    ASSERT_GL_ERROR();
    assert(shader->vertPositionLocation() != -1);
    glEnableVertexAttribArray(shader->vertPositionLocation());
    ASSERT_GL_ERROR();
    glVertexAttribPointer(
        shader->vertPositionLocation(),  // index
        3,  // size
        GL_FLOAT,  // type
        0,  // normalized
        sizeof(Vertex),  // stride
        &(((Vertex *)0)->pos[0])  // pointer
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
        sizeof(Vertex),  // stride
        &(((Vertex *)0)->color[0])  // pointer
        );
    ASSERT_GL_ERROR();

    // Draw the points for debugging
    glPointSize(3.0f);
    ASSERT_GL_ERROR();
    glDrawArrays(
        GL_POINTS,  // mode
        0,  // first
        m_numPoints  // count
        );
    ASSERT_GL_ERROR();
  }

  void CubeObject::draw(const glm::mat4 &modelWorld,
      const glm::mat4 &worldView, const glm::mat4 &projection,
      float alpha, bool debug) const
  {
    glm::mat4 modelView = worldView * modelWorld;

    // Draw the cube wireframe
    m_drawCubeWireframe(modelView, projection);

    // Draw the isosurface triangles
    m_drawTriangleWireframe(modelView, projection);

    // Draw the lattice points and edge intersection points
    m_drawDebugPoints(modelView, projection);
  }

  void CubeObject::setCube(unsigned int cube) {
    m_cube = cube;

    // TODO: (Re-)evaluate the isosurface extraction algorithm, since we have a
    //       new isosurface
    CubeScalarField sf(cube);
    const Mesh *result = m_builder.buildIsosurface(
        sf,
        MC_SIMPLE_MARCHING_CUBES
        );

    // Copy the resulting mesh into m_mesh
    m_mesh = *result;

    // Generate point data to send to the GL for visual debugging
    m_generateDebugPoints(result);

    // Generate triangle wireframe data and send it to the GL
    m_generateTriangleWireframe(result);
  }

  CubeObject::CubeScalarField::CubeScalarField(unsigned int cube)
    : m_cube(cube)
  {
  }

  // FIXME: This code was copied from src/mc/tables/common.c
  int vertexValue(unsigned int vertex, unsigned int cube) {
    return (cube & (1 << vertex)) >> vertex;
  }

  // FIXME: This code was copied from src/mc/tables/common.c
  unsigned int vertexIndex(unsigned int x, unsigned int y, unsigned int z) {
    assert((x & ~1) == 0);
    assert((y & ~1) == 0);
    assert((z & ~1) == 0);
    int i = x | y << 1 | z << 2;
    static const unsigned int table[] = {
      0,  // x = 0, y = 0, z = 0
      1,  // x = 1, y = 0, z = 0
      3,  // x = 0, y = 1, z = 0
      2,  // x = 1, y = 1, z = 0
      4,  // x = 0, y = 0, z = 1
      5,  // x = 1, y = 0, z = 1
      7,  // x = 0, y = 1, z = 1
      6   // x = 1, y = 1, z = 1
    };
    return table[i];
  }

  float CubeObject::CubeScalarField::operator()(
      float x, float y, float z)
  {
    // A tri-linear interpolation between the cube vertices
    float result = 0.0f;
    for (unsigned int z_index = 0; z_index <= 1; ++z_index) {
      for (unsigned int y_index = 0; y_index <= 1; ++y_index) {
        for (unsigned int x_index = 0; x_index <= 1; ++x_index) {
          unsigned int i = mcSimpleVertexIndex(x_index, y_index, z_index);
          float value = vertexValue(i, m_cube) ? 1.0f : -1.0f;
          result +=
            (x_index ? x : 1.0f - x) *
            (y_index ? y : 1.0f - y) *
            (z_index ? z : 1.0f - z) *
            value;
        }
      }
    }
    return result;
  }
} } }
