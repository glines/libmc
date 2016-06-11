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
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mcxx/mesh.h>
#include <mcxx/vector.h>

#include "../common/glError.h"
#include "../common/shaderProgram.h"

#include "scanObject.h"

namespace mc { namespace samples {
  ScanObject::ScanObject(
      const std::string &file,
      unsigned int res_x, unsigned int res_y, unsigned int res_z,
      mcAlgorithmFlag algorithm,
      const glm::vec3 &position, const glm::quat &orientation)
    : SceneObject(position, orientation),
      m_scan(file)  // Read the scan from image files
  {
    m_init();

    // Update the isosurface mesh
    m_update();
  }

  void ScanObject::m_init() {
    // Create names for our GL buffers
    glGenBuffers(1, &m_wireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glGenBuffers(1, &m_wireframeIndices);
    FORCE_ASSERT_GL_ERROR();
  }

  void ScanObject::m_generateWireframe(const Mesh *mesh) {
    // Copy the vertices from the mesh
    WireframeVertex *vertices = new WireframeVertex[mesh->numVertices()];
    for (unsigned int i = 0; i < mesh->numVertices(); ++i) {
      auto vertex = mesh->vertex(i);
      vertices[i].pos[0] = vertex.pos.x;
      vertices[i].pos[1] = vertex.pos.y;
      vertices[i].pos[2] = vertex.pos.z;
      vertices[i].color[0] = 1.0f;
      vertices[i].color[1] = 1.0f;
      vertices[i].color[2] = 1.0f;
    }
    // Send the vertices to the GL
    glBindBuffer(GL_ARRAY_BUFFER, m_wireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  // target
        sizeof(WireframeVertex) * mesh->numVertices(),  // size
        vertices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
    delete[] vertices;
    // Make lines for the face edges in the mesh
    unsigned int *indices = new unsigned int[mesh->numIndices() * 2];
    unsigned int currentIndex = 0;
    for (unsigned int i = 0; i < mesh->numFaces(); ++i) {
      auto face = mesh->face(i);
      for (unsigned int j = 0; j < face.numIndices; ++j) {
        assert(currentIndex < mesh->numIndices() * 2);
        indices[currentIndex++] = face.indices[j];
        assert(currentIndex < mesh->numIndices() * 2);
        indices[currentIndex++] = face.indices[(j + 1) % face.numIndices];
      }
    }
    // Send the line indices to the GL
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_wireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  // target
        sizeof(unsigned int) * mesh->numIndices() * 2,  // size
        indices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
    delete[] indices;
    m_numWireframeLines = mesh->numIndices();
  }

  void ScanObject::m_update() {
    // TODO: Build a mesh representing the isosurface
    // FIXME: The contour value needs to be specified
    // TODO: Allow the isosurface extraction algorithm to be specified
    // TODO: Allow the resolution to be specified
    auto mesh = m_isosurfaceBuilder.buildIsosurface(
        m_scan.scalarField(),  // scalarField
        MC_SIMPLE_MARCHING_CUBES,  // algorithm
        m_scan.xRes(), m_scan.yRes(), m_scan.zRes(),  // res
        Vec3(-1.0f, -1.0f, -1.0f),  // min
        Vec3(1.0f, 1.0f, 1.0f)  // max
        );
    fprintf(stderr, "mesh->numVertices(): %d\n", mesh->numVertices());

    // Generate wireframe data and send it to the GL
    m_generateWireframe(mesh);
  }

#ifdef __EMSCRIPTEN__
#define SHADER_DIR "./assets/shaders/webgl"
#else
#define SHADER_DIR "./assets/shaders/glsl"
#endif

#define DEFINE_SHADER(shader) \
  std::shared_ptr<ShaderProgram> ScanObject::m_ ## shader ## Shader() { \
    static std::shared_ptr<ShaderProgram> instance = \
      std::shared_ptr<ShaderProgram>( \
          new ShaderProgram( \
            SHADER_DIR "/" #shader ".vert", \
            SHADER_DIR "/" #shader ".frag" \
            )); \
    return instance; \
  }

  DEFINE_SHADER(wireframe)

  void ScanObject::m_drawWireframe(
      const glm::mat4 &modelView, const glm::mat4 &projection) const
  {
    // Use our shader for drawing wireframes
    std::shared_ptr<ShaderProgram> shader = m_wireframeShader();
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
    glBindBuffer(GL_ARRAY_BUFFER, m_wireframeVertices);
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

    // Draw the wireframe lines
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_wireframeIndices);
    ASSERT_GL_ERROR();
    glLineWidth(1.0f);
    ASSERT_GL_ERROR();
    glDrawElements(
        GL_LINES,  // mode
        m_numWireframeLines * 2,  // count
        GL_UNSIGNED_INT,  // type
        0  // indices
        );
    ASSERT_GL_ERROR();
  }

  void ScanObject::draw(const glm::mat4 &modelWorld,
      const glm::mat4 &worldView, const glm::mat4 &projection,
      float alpha, bool debug) const
  {
    // Compute the matrices we need for the shaders
    glm::mat4 modelView = worldView * modelWorld;
    glm::mat4 modelViewProjection = projection * modelView;
    glm::mat4 normalTransform = glm::inverseTranspose(modelView);

    // Draw the mesh wireframe
    m_drawWireframe(modelView, projection);
  }
} }
