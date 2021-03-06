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

#ifndef MC_SAMPLES_CUBES_CUBE_OBJECT_H_
#define MC_SAMPLES_CUBES_CUBE_OBJECT_H_

#include <GL/glew.h>
#include <mcxx/isosurfaceBuilder.h>
#include <mcxx/mesh.h>
#include <mcxx/scalarField.h>
#include <memory>

#include "../common/meshObject.h"

namespace mc { namespace samples { namespace cubes {
  /**
   * Debugging class for visualizing voxel cube configurations and the
   * surfaces they generate. This class is particularly useful because it can
   * interpolate between lattice sample points to arbitrary resolution, which
   * is an effective tool for visualizing ambiguities present in certain cube
   * configurations.
   */
  class CubeObject : public MeshObject {
    private:
      IsosurfaceBuilder m_builder;
      Mesh m_mesh;
      unsigned int m_cube;
      GLuint m_cubeWireframeVertices, m_cubeWireframeIndices,
             m_pointBuffer;
      unsigned int m_numPoints;
      unsigned int m_resX, m_resY, m_resZ;
      mcAlgorithmFlag m_algorithm;
      bool m_isDrawScalarField;
      float m_intensity;

      void m_generateCubeWireframe();
      void m_generateDebugPoints(const Mesh *mesh);

      void m_update();

      void m_drawCubeWireframe(
          const glm::mat4 &modelView,
          const glm::mat4 &projection) const;
      void m_drawDebugPoints(
          const glm::mat4 &modelView,
          const glm::mat4 &projection) const;

      typedef struct WireframeVertex {
        float pos[3];
        float color[3];
      } WireframeVertex;

      class CubeScalarField : public ScalarField {
        private:
          unsigned int m_cube;
          float m_intensity;
        public:
          CubeScalarField(
              unsigned int cube,
              float intensity = 1.0f);

          float operator()(float x, float y, float z);
      };
    public:
      /**
       * A scene object that represents a single voxel cube. This is used to
       * visualize individual voxels for debugging purposes.
       *
       * \param cube The bitwise representation of the voxel cube configuration.
       * \param res_x The x-axis resolution at which to sample for the
       * isosurface mesh.
       * \param res_y The y-axis resolution at which to sample for the
       * isosurface mesh.
       * \param res_z The z-axis resolution at which to sample for the
       * isosurface mesh.
       * \param algorithm Flag representing the isosurface extraction
       * algorithm to use to generate the isosurface mesh.
       * \param position Position of the cube object in the scene.
       * \param orientation Orientation of the cube object in the scene.
       */
      CubeObject(
          unsigned int cube,
          unsigned int res_x = 10,
          unsigned int res_y = 10,
          unsigned int res_z = 10,
          mcAlgorithmFlag algorithm = MC_SIMPLE_MARCHING_CUBES,
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat());

      /**
       * Draws the cube voxel, its vertices, edges, and the triangles that make
       * up its mesh.
       */
      void draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug);

      /**
       * Returns the index of the cube being represented by this cube object.
       */
      unsigned int cube() const { return m_cube; }

      /**
       * Changes the cube being represented by this cube object. Calling this
       * method will change the underlying isosurface.
       *
       * \param cube The bitwise voxel cube representation to use to define
       * the trilineraly interpolated implicit isosurface function.
       */
      void setCube(unsigned int cube);

      /**
       * Returns true if the scalar field is being drawn as a lattice of
       * points. Returns false otherwise.
       *
       * \return Whether or not the scalar field is being drawn as a lattice
       * of points.
       */
      bool isDrawScalarField() const {
        return m_isDrawScalarField;
      }

      /**
       * Sets whether or not the scalar field is to be drawn as a lattice of
       * points.
       */
      void setDrawScalarField(bool flag) {
        m_isDrawScalarField = flag;
      }

      /**
       * Sets the resolution used with the isosurface extraction algorithm.
       * Higher resolution gives more detail and generates more triangles.
       *
       * \param x The x-axis resolution of the sample lattice to use when
       * generating the isosurface mesh.
       * \param y The y-axis resolution of the sample lattice to use when
       * generating the isosurface mesh.
       * \param z The z-axis resolution of the sample lattice to use when
       * generating the isosurface mesh.
       */
      void setResolution(
          unsigned int x, unsigned int y, unsigned int z);

      /**
       * Sets the algorithm used for isosurface extraction. Calling this
       * method will cause the isosurface mesh to be re-evaluated.
       *
       * \param algorithm Flag representing the isosurface extraction
       * algorithm to use to generate the isosurface mesh.
       */
      void setAlgorithm(mcAlgorithmFlag algorithm);

      /**
       * Returns the intensity value for the isosurface generated, which
       * affects how the edge values are interpolated.
       */
      float intensity() const { return m_intensity; }

      /**
       * Sets the intensity value for the isosurface generated, which
       * affects how the edge values are interpolated. The isosurface is
       * re-evaluated with each call to this method.
       *
       * \param intensity The intensity of the sample values under the
       * surface relative to the sample values above the isosurface.
       */
      void setIntensity(float intensity);
  };
} } }

#endif
