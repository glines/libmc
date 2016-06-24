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

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glError.h"
#include "shaderProgram.h"
#include "shaders.h"

#include "meshObject.h"

namespace mc { namespace samples {
  MeshObject::MeshObject(
      const glm::vec3 &position,
      const glm::quat &orientation)
    : SceneObject(position, orientation),
      m_isDrawWireframe(true), m_isDrawNormals(false), m_isDrawOpaque(true)
  {
    // Initialize our GL buffers
    glGenBuffers(1, &m_wireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glGenBuffers(1, &m_wireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glGenBuffers(1, &m_surfaceNormalVertices);
    FORCE_ASSERT_GL_ERROR();
    glGenBuffers(1, &m_vertexBuffer);
    FORCE_ASSERT_GL_ERROR();
    glGenBuffers(1, &m_indexBuffer);
    FORCE_ASSERT_GL_ERROR();

    // FIXME: The mc::Mesh class does not support empty mesh construction
    /*
    // Initialize our mesh data with an empty mesh
    mc::Mesh mesh;
    this->setMesh(mesh);
    */
  }

  MeshObject::~MeshObject() {}

  void MeshObject::setMesh(const mc::Mesh &mesh) {
    m_numVertices = mesh.numVertices();

    // Generate wireframe data and send it to the GL
    m_generateWireframe(mesh);

    // Generate surface normal lines and send them to the GL
    m_generateSurfaceNormals(mesh);

    // Send trangles representing the mesh surface to the GL
    m_generateTriangles(mesh);
  }

  void MeshObject::m_generateTriangles(const Mesh &mesh) {
    // Copy the vertices from the mesh to a buffer
    Vertex *vertices = new Vertex[mesh.numVertices()];
    for (int i = 0; i < mesh.numVertices(); ++i) {
      auto vertex = mesh.vertex(i);
      vertices[i].pos[0] = vertex.pos.x;
      vertices[i].pos[1] = vertex.pos.y;
      vertices[i].pos[2] = vertex.pos.z;
      vertices[i].norm[0] = vertex.norm.x;
      vertices[i].norm[1] = vertex.norm.y;
      vertices[i].norm[2] = vertex.norm.z;
    }
    // Send the vertices to the GL
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  // target
        sizeof(Vertex) * mesh.numVertices(),  // size
        vertices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
    delete[] vertices;
    // Copy the triangle indices from the mesh to a buffer
    unsigned int numIndices;
    if (mesh.isTriangleMesh()) {
      numIndices = mesh.numFaces() * 3;
    } else {
      // Determine how many indices are needed in order to triangulate this
      // mesh
      numIndices = 0;
      for (int i = 0; i < mesh.numFaces(); ++i) {
        auto face = mesh.face(i);
        assert(face.numIndices >= 3);
        numIndices += 3 + (face.numIndices - 3) * 3;
      }
    }
    unsigned int *indices = new unsigned int [numIndices];
    unsigned int currentIndex = 0;
    m_numTriangles = 0;
    for (int i = 0; i < mesh.numFaces(); ++i) {
      auto face = mesh.face(i);
      assert(face.numIndices >= 3);
      for (int j = 0; j < 3; ++j) {
        assert(currentIndex < numIndices);
        indices[currentIndex++] = face.indices[j];
      }
      m_numTriangles += 1;
      for (int j = 3; j < face.numIndices; ++j) {
        /* Draw the remaining parts of the face with a triangle fan. This might
         * not make optimal geometry, but this is acceptable for a debugging
         * program. */
        assert(currentIndex < numIndices);
        indices[currentIndex++] = face.indices[0];
        assert(currentIndex < numIndices);
        indices[currentIndex++] = face.indices[j - 1];
        assert(currentIndex < numIndices);
        indices[currentIndex++] = face.indices[j];
        m_numTriangles += 1;
      }
    }
    // Send the indices to the GL
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  // target
        sizeof(unsigned int) * numIndices,  // size
        indices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
    delete[] indices;
  }

  void MeshObject::m_generateWireframe(const Mesh &mesh) {
    // Copy the vertices from the mesh
    WireframeVertex *vertices = new WireframeVertex[mesh.numVertices()];
    for (unsigned int i = 0; i < mesh.numVertices(); ++i) {
      auto vertex = mesh.vertex(i);
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
        sizeof(WireframeVertex) * mesh.numVertices(),  // size
        vertices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
    delete[] vertices;
    unsigned int *indices = new unsigned int[mesh.numIndices() * 2];
    unsigned int currentIndex = 0;
    // Create lines from the face indices of the mesh
    for (unsigned int i = 0; i < mesh.numFaces(); ++i) {
      auto face = mesh.face(i);
      for (unsigned int j = 0; j < face.numIndices; ++j) {
        assert(currentIndex < mesh.numIndices() * 2);
        indices[currentIndex++] = face.indices[j];
        assert(currentIndex < mesh.numIndices() * 2);
        indices[currentIndex++] = face.indices[(j + 1) % face.numIndices];
      }
    }
    // Send the line indices to the GL
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_wireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  // target
        sizeof(unsigned int) * mesh.numIndices() * 2,  // size
        indices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
    delete[] indices;
    m_numWireframeLines = mesh.numIndices();
  }

  void MeshObject::m_generateSurfaceNormals(const Mesh &mesh) {
    // Allocate memory for the surface normal lines
    WireframeVertex *lines = new WireframeVertex[mesh.numVertices() * 2];
    // Iterate through the mesh vertices
    for (unsigned int i = 0; i < mesh.numVertices(); ++i) {
      // Make a line to represent the surface normal
      auto v = mesh.vertex(i);
      lines[i * 2].pos[0] = v.pos.x;
      lines[i * 2].pos[1] = v.pos.y;
      lines[i * 2].pos[2] = v.pos.z;
      lines[i * 2 + 1].pos[0] = v.pos.x + v.norm.x;
      lines[i * 2 + 1].pos[1] = v.pos.y + v.norm.y;
      lines[i * 2 + 1].pos[2] = v.pos.z + v.norm.z;
      lines[i * 2].color[0] = 0.0f;
      lines[i * 2].color[1] = 0.0f;
      lines[i * 2].color[2] = 1.0f;
      lines[i * 2 + 1].color[0] = 0.0f;
      lines[i * 2 + 1].color[1] = 0.0f;
      lines[i * 2 + 1].color[2] = 1.0f;
    }
    // Copy the surface normal lines to the GL
    glBindBuffer(GL_ARRAY_BUFFER, m_surfaceNormalVertices);
    glBufferData(
        GL_ARRAY_BUFFER,  // target
        sizeof(WireframeVertex) * mesh.numVertices() * 2,  // size
        lines,  // data
        GL_STATIC_DRAW  // usage
        );

    delete[] lines;
  }

  void MeshObject::m_drawWireframe(
      const glm::mat4 &modelView,
      const glm::mat4 &projection) const
  {
    // Use our shader for drawing wireframes
    std::shared_ptr<ShaderProgram> shader = Shaders::wireframeShader();
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

  void MeshObject::m_drawSurfaceNormals(
      const glm::mat4 &modelView,
      const glm::mat4 &projection) const
  {
    // Use our shader for drawing wireframes
    std::shared_ptr<ShaderProgram> shader = Shaders::wireframeShader();
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
    glBindBuffer(GL_ARRAY_BUFFER, m_surfaceNormalVertices);
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

    // Draw the surface normals
    glLineWidth(1.0f);
    ASSERT_GL_ERROR();
    glDrawArrays(
        GL_LINES,  // mode
        0,  // first
        m_numVertices * 2  // count
        );
    ASSERT_GL_ERROR();
  }

  void MeshObject::m_drawSurface(
      const glm::mat4 &modelView,
      const glm::mat4 &projection,
      const glm::mat4 &modelViewProjection,
      const glm::mat4 &normalTransform) const
  {
    // TODO: Support Gouraud, Phong, and flat shading
    // TODO: Support specular highlights
    // Use the Gouraud shader program
    auto shader = Shaders::gouraudShader();
    shader->use();

    // Prepare the uniform values
    /*
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
    */
    assert(shader->modelViewProjectionLocation() != -1);
    glUniformMatrix4fv(
        shader->modelViewProjectionLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(modelViewProjection)  // value
        );
    ASSERT_GL_ERROR();
    assert(shader->normalTransformLocation() != -1);
    glUniformMatrix4fv(
        shader->normalTransformLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(normalTransform)  // value
        );
    ASSERT_GL_ERROR();
    // Some uniforms for lighting
    assert(shader->lightPositionLocation() != -1);
    glUniform3f(
        shader->lightPositionLocation(),
        3.0f, 3.0f, 3.0f);
    ASSERT_GL_ERROR();
    assert(shader->lightIntensityLocation() != -1);
    glUniform3f(
        shader->lightIntensityLocation(),
        1.0f, 1.0f, 1.0f);
    ASSERT_GL_ERROR();

    // Prepare the vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
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
    assert(shader->vertNormalLocation() != -1);
    glEnableVertexAttribArray(shader->vertNormalLocation());
    ASSERT_GL_ERROR();
    glVertexAttribPointer(
        shader->vertNormalLocation(),  // index
        3,  // size
        GL_FLOAT,  // type
        0,  // normalized
        sizeof(Vertex),  // stride
        &(((Vertex *)0)->norm[0])  // pointer
        );
    ASSERT_GL_ERROR();

    // Draw the isosurface
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    ASSERT_GL_ERROR();
    glEnable(GL_POLYGON_OFFSET_FILL);
    ASSERT_GL_ERROR();
    glPolygonOffset(1.0f, 1.0f);
    ASSERT_GL_ERROR();
    glDrawElements(
        GL_TRIANGLES,  // mode
        m_numTriangles * 3,  // count
        GL_UNSIGNED_INT,  // type
        0  // indices
        );
    ASSERT_GL_ERROR();
    glDisable(GL_POLYGON_OFFSET_FILL);
    ASSERT_GL_ERROR();
  }

  void MeshObject::draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug) const
  {
    // Compute the matrices we need for the shaders
    glm::mat4 modelView = worldView * modelWorld;
    glm::mat4 modelViewProjection = projection * modelView;
    glm::mat4 normalTransform = glm::inverseTranspose(modelView);

    if (m_isDrawWireframe) {
      // Draw the mesh faces as a wireframe
      m_drawWireframe(modelView, projection);
    }

    if (m_isDrawNormals) {
      // Draw the surface normals
      m_drawSurfaceNormals(modelView, projection);
    }
    
    if (m_isDrawOpaque) {
      // Draw the surface with opaque rendering
      m_drawSurface(modelView, projection, modelViewProjection,
          normalTransform);
    }
  }
} }
