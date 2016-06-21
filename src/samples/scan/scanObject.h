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

#ifndef MC_SAMPLES_SCAN_SCAN_OBJECT_H_
#define MC_SAMPLES_SCAN_SCAN_OBJECT_H_

/**
 * \addtogroup samples
 */

/**
 * \addtogroup scan
 */

#include <GL/glew.h>
#include <mcxx/isosurfaceBuilder.h>

#include "../common/meshObject.h"
#include "scan.h"

namespace mc { namespace samples {
  class ShaderProgram;
  /**
   * Mesh object representing a 3D scan, such as a CT scan.
   */
  class ScanObject : public MeshObject {
    private:
      unsigned int m_resX, m_resY, m_resZ;
      mcAlgorithmFlag m_algorithm;
      Scan m_scan;
      IsosurfaceBuilder m_isosurfaceBuilder;

      void m_init();

      void m_update();

    public:
      /**
       * Constructs a scan object which represents an isosurface of an
       * underlying 3D scan with a mesh.
       *
       * \param path Path to a directory containing slices of the scan as PNG
       * image files in alphabetical order.
       * \param res_x The x-axis resolution of the lattice grid to sample for
       * the isosurface mesh.
       * \param res_y The y-axis resolution of the lattice grid to sample for
       * the isosurface mesh.
       * \param res_z The z-axis resolution of the lattice grid to sample for
       * the isosurface mesh.
       * \param algorithm Flag representing the isosurface extraction algorithm
       * to be used for generating the isosurface mesh.
       * \param position Position of the scan object.
       * \param orientation Orientation of the scan object.
       */
      ScanObject(
          const std::string &path,
          unsigned int res_x = 10,
          unsigned int res_y = 10,
          unsigned int res_z = 10,
          mcAlgorithmFlag algorithm = MC_SIMPLE_MARCHING_CUBES,
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat());
      /**
       * Destroy this scan object.
       */
      ~ScanObject();
  };
} }

/** @} */

/** @} */

#endif
