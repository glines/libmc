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

#include <mcxx/isosurfaceBuilder.h>

#include "isosurfaceObject.h"

namespace mc { namespace samples {
  IsosurfaceObject::IsosurfaceObject(
      const glm::vec3 &position,
      const glm::quat &orientation)
    : MeshObject(position, orientation),
    m_algorithm(MC_ORIGINAL_MARCHING_CUBES),
    m_res(8),
    m_min(0.0f, 0.0f, 0.0f),
    m_max(1.0f, 1.0f, 1.0f)
  {
  }

  void IsosurfaceObject::m_update() {
    if (!m_sf)
      return;
    IsosurfaceBuilder ib;
    auto mesh = ib.buildIsosurface(
        *m_sf,  // scalar field
        m_algorithm,  // algorithm
        m_res * (m_max.x() - m_min.x()),  // x resolution
        m_res * (m_max.y() - m_min.y()),  // y resolution
        m_res * (m_max.z() - m_min.z()),  // z resolution
        m_min,  // min
        m_max  // max
        );
    this->setMesh(*mesh);
  }

  void IsosurfaceObject::setAlgorithm(mcAlgorithmFlag algorithm) {
    m_algorithm = algorithm;
    m_update();
  }

  void IsosurfaceObject::setScalarField(std::shared_ptr<ScalarField> sf) {
    m_sf = sf;
    m_update();
  }

  void IsosurfaceObject::setMin(const Vec3 &min) {
    m_min = min;
    m_update();
  }

  void IsosurfaceObject::setMax(const Vec3 &max) {
    m_max = max;
    m_update();
  }
} }
