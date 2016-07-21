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

extern "C" {
#include <mc/mesh.h>
}

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
      typedef struct MeshStruct {
        typedef struct Vertex {
          float pos[3];
          float norm[3];
        } Vertex;
        typedef struct WireframeVertex {
          float pos[3];
          float color[3];
        } WireframeVertex;

        Vertex *triangleVertices;
        unsigned int *triangleIndices;
        WireframeVertex *wireframeVertices;
        unsigned int *wireframeIndices;
        WireframeVertex *surfaceNormalVertices;
        unsigned int numVertices, numTriangles, numWireframeLines;

        void initialize(const mcMesh &mesh);
        void m_initializeTriangles(const mcMesh &mesh);
        void m_initializeWireframe(const mcMesh &mesh);
        void m_initializeSurfaceNormals(const mcMesh &mesh);
        void destroy();
      } MeshStruct;

      MeshStruct *m_meshStruct;

      GLuint m_wireframeVertices, m_wireframeIndices,
             m_surfaceNormalVertices,
             m_vertexBuffer, m_indexBuffer;
      unsigned int m_numTriangles, m_numVertices,
                   m_numWireframeLines;
      bool m_isDrawWireframe, m_isDrawNormals, m_isDrawOpaque, m_isInitialized;

      void m_uploadMesh(const MeshStruct &mesh);
      void m_initGl();
      void m_uploadTriangles(const MeshStruct &mesh);
      void m_uploadWireframe(const MeshStruct &mesh);
      void m_uploadSurfaceNormals(const MeshStruct &mesh);

      void m_update();

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

    public:
      /**
       * Constructs a mesh object with the given position and orientation.
       *
       * \param position The scene position of the mesh object.
       * \param orientation The scene orientation of the mesh object.
       */
      MeshObject(
          const glm::vec3 &position,
          const glm::quat &orientation);
      /**
       * Destroys the mesh object.
       */
      ~MeshObject();

      /**
       * Draw the mesh being represented by this mesh object.
       *
       * \param modelWorld The model-space to world-space transform for this
       * scene object's position and orientation.
       * \param worldView The world-space to view-space transform for the
       * position and orientation of the camera currently being used.
       * \param projection The view-space to projection-space transform for
       * the camera that is currently being used.
       * \param alpha The simulation keyframe weight for animating this object
       * between keyframes.
       * \param debug Flag indicating whether or not debug information is to be
       * drawn.
       */
      virtual void draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug);

      /**
       * Returns true if the the surface normals are being drawn (as lines).
       * Returns false otherwise.
       *
       * \return Whether or not surface normal lines are being drawn.
       */
      bool isDrawNormals() const {
        return m_isDrawNormals;
      }

      /**
       * Sets whether or not surface normal lines are to be drawn.
       *
       * \param flag Whether or not surface normal lines are to be drawn.
       */
      void setDrawNormals(bool flag) {
        m_isDrawNormals = flag;
      }

      /**
       * Returns true if the the wireframe of the mesh is being drawn. Returns
       * false otherwise.
       *
       * \return Whether or not the mesh wireframe is being drawn.
       */
      bool isDrawWireframe() const {
        return m_isDrawWireframe;
      }

      /**
       * Sets whether or not a wireframe of the mesh is to be drawn.
       *
       * \param flag Whether or not to draw the mesh wireframe.
       */
      void setDrawWireframe(bool flag) {
        m_isDrawWireframe = flag;
      }

      /**
       * Returns true if the opaque surface (with lighting) is being drawn.
       * Returns false otherwise.
       *
       * \return Whether or not the opaque surface is being drawn.
       */
      bool isDrawOpaque() {
        return m_isDrawOpaque;
      }

      /**
       * Sets whether or not the opaque surface is to be drawn.
       *
       * \param flag Whether or not to draw the opaque surface.
       */
      void setDrawOpaque(bool flag) {
        m_isDrawOpaque = flag;
      }

    protected:
      /**
       * Sets the mesh to be drawn by this mesh object. The mesh is copied, but
       * it is not uploaded to the GL until the next draw call. This is because
       * we cannot risk making GL calls outside of the draw thread.
       */
      void setMesh(const mc::Mesh &mesh);

      void setMesh(const mcMesh &mesh);
  };
} }

#endif
