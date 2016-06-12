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
    : MeshObject(position, orientation),
      m_scan(file),  // Read the scan from image files
      m_resX(res_x), m_resY(res_y), m_resZ(res_z)
  {
    // Update the isosurface mesh
    m_update();
  }

  ScanObject::~ScanObject() {}

  void ScanObject::m_update() {
    // TODO: Build a mesh representing the isosurface
    // FIXME: The contour value needs to be specified
    // TODO: Allow the isosurface extraction algorithm to be specified
    // TODO: Allow the resolution to be specified
    auto mesh = m_isosurfaceBuilder.buildIsosurface(
        m_scan.scalarField(),  // scalarField
        MC_SIMPLE_MARCHING_CUBES,  // algorithm
        m_resX, m_resY, m_resZ,  // res
        Vec3(-1.0f, -1.0f, -1.0f),  // min
        Vec3(1.0f, 1.0f, 1.0f)  // max
        );
    fprintf(stderr, "mesh->numVertices(): %d\n", mesh->numVertices());

    // Generate wireframe data and send it to the GL
    this->setMesh(*mesh);
  }
} }
