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

#ifndef MC_SAMPLES_COMMON_MESH_OBJECT_H_
#define MC_SAMPLES_COMMON_MESH_OBJECT_H_

#include <GL/glew.h>
#include <mcxx/mesh.h>

#include "sceneObject.h"

namespace mc { namespace samples {
  class ShaderProgram;
  /**
   * A scene object capable of representing a mc::Mesh object.
   *
   * Multiple options are provided for drawing the wireframe, normals, and
   * surfaces with numerous shading methods.
   */
  class MeshObject : public SceneObject {
    private:
      GLuint m_wireframeVertices, m_wireframeIndices,
             m_surfaceNormalVertices,
             m_vertexBuffer, m_indexBuffer;
      unsigned int m_numTriangles, m_numVertices,
                   m_numWireframeLines;
      bool m_isDrawWireframe, m_isDrawNormals, m_isDrawOpaque;

      void m_generateTriangles(const Mesh &mesh);
      void m_generateWireframe(const Mesh &mesh);
      void m_generateSurfaceNormals(const Mesh &mesh);

      void m_update();

#define DECLARE_SHADER(shader) \
        static std::shared_ptr<ShaderProgram> m_ ## shader ## Shader()

      DECLARE_SHADER(wireframe);
      DECLARE_SHADER(gouraud);
      DECLARE_SHADER(phong);

      void m_drawWireframe(
          const glm::mat4 &modelView,
          const glm::mat4 &projection) const;
      void m_drawSurfaceNormals(
          const glm::mat4 &modelView,
          const glm::mat4 &projection) const;
      void m_drawSurface(
          const glm::mat4 &modelView,
          const glm::mat4 &projection,
          const glm::mat4 &modelViewProjection,
          const glm::mat4 &normalTransform) const;

      typedef struct Vertex {
        float pos[3];
        float norm[3];
      } Vertex;
      typedef struct WireframeVertex {
        float pos[3];
        float color[3];
      } WireframeVertex;

    public:
      MeshObject(
          const glm::vec3 &position,
          const glm::quat &orientation);
      ~MeshObject();

      /**
       * Draw the mesh being represented by this mesh object.
       */
      virtual void draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug) const;

      /**
       * Returns true if the the surface normals are being drawn (as lines).
       * Returns false otherwise.
       */
      bool isDrawNormals() const {
        return m_isDrawNormals;
      }

      /**
       * Sets whether or not surface normal lines are to be drawn.
       */
      void setDrawNormals(bool flag) {
        m_isDrawNormals = flag;
      }

      /**
       * Returns true if the the wireframe of the triangle mesh is being
       * drawn. Returns false otherwise.
       */
      bool isDrawWireframe() const {
        return m_isDrawWireframe;
      }

      /**
       * Sets whether or not a wireframe of the triangle mesh is to be drawn.
       */
      void setDrawWireframe(bool flag) {
        m_isDrawWireframe = flag;
      }

      /**
       * Returns true if the opaque surface (with lighting) is being drawn.
       * Returns false otherwise.
       */
      bool isDrawOpaque() {
        return m_isDrawOpaque;
      }

      /**
       * Sets whether or not the opaque surface is to be drawn.
       */
      void setDrawOpaque(bool flag) {
        m_isDrawOpaque = flag;
      }

    protected:
      /**
       * Updates the mesh being represented by the mesh object by copying new
       * data to the GL.
       */
      void setMesh(const mc::Mesh &mesh);
  };
} }

#endif
