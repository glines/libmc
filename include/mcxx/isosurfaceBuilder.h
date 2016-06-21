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

#ifndef MCXX_ISOSURFACE_BUILDER_H_
#define MCXX_ISOSURFACE_BUILDER_H_

#include <vector>

extern "C" {
#include <mc/isosurfaceBuilder.h>
}

namespace mc {
  class Mesh;
  class ScalarField;
  class Vec3;
  /**
   * C++ wrapper for the mcIsosurfaceBuilder C struct. This class facilitates
   * the construction of Mesh objects from various given representations of
   * isosurfaces. Constructing a mesh with this builder class implies
   * allocation of memory for that mesh, and destroying this builder implies
   * the de-allocation of that memory.
   *
   * \sa mcIsosurfaceBuilder
   */
  class IsosurfaceBuilder {
    private:
      mcIsosurfaceBuilder m_internal;
      std::vector<Mesh*> m_meshes;

      static float m_wrapScalarField(
          float x, float y, float z, const ScalarField *sf);
    public:
      IsosurfaceBuilder();
      ~IsosurfaceBuilder();

      /**
       * Builds a mesh representing the isosurface defined by \p sf using the
       * algorithm given by \p algorithm.
       *
       * \param sf The scalar field function defining the underlying isosurface.
       * \param algorithm A flag representing the isosurface extraction
       * algorithm to be used.
       * \param args Auxilliary arguments to be passed to the scalar field
       * function, which facilitates things like functors.
       * \return A mesh representing the isosurface.
       *
       * \todo I don't think that \p args is needed for this method.
       */
      const Mesh *buildIsosurface(
          mcScalarField sf,
          mcAlgorithmFlag algorithm,
          void *args = nullptr
          );

      /**
       * Builds a mesh representing the isosurface defined by \p sf using the
       * algorithm given by \p algorithm.
       *
       * \param sf The scalar field \em functor defining the underlying
       * isosurface.
       * \param algorithm A flag representing the isosurface extraction
       * algorithm to be used.
       * \param x_res The number of samples to take in the sample lattice
       * parallel to the x-axis.
       * \param y_res The number of samples to take in the sample lattice
       * parallel to the y-axis.
       * \param z_res The number of samples to take in the sample lattice
       * parallel to the z-axis.
       * \param min The absolute position where the sample lattice begins and
       * the first sample is to be taken.
       * \param max The absolute position where the sample lattice ends and the
       * last sample is to be taken.
       * \return A mesh representing the isosurface.
       */
      const Mesh *buildIsosurface(
          const ScalarField &sf,
          mcAlgorithmFlag algorithm,
          unsigned int x_res, unsigned int y_res, unsigned int z_res,
          const Vec3 &min, const Vec3 &max);
  };
}

#endif
