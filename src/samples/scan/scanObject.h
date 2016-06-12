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

#include <GL/glew.h>
#include <mcxx/isosurfaceBuilder.h>

#include "../common/meshObject.h"
#include "scan.h"

namespace mc { namespace samples {
  class ShaderProgram;
  class ScanObject : public MeshObject {
    private:
      unsigned int m_resX, m_resY, m_resZ;
      mcAlgorithmFlag m_algorithm;
      Scan m_scan;
      IsosurfaceBuilder m_isosurfaceBuilder;

      void m_init();

      void m_update();

    public:
      ScanObject(
          const std::string &file,
          unsigned int res_x = 10,
          unsigned int res_y = 10,
          unsigned int res_z = 10,
          mcAlgorithmFlag algorithm = MC_SIMPLE_MARCHING_CUBES,
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat());
      ~ScanObject();
  };
} }

#endif
