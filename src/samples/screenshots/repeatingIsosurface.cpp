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

#include <cmath>
#include <mcxx/isosurfaceBuilder.h>
#include <mcxx/vector.h>

#include "repeatingIsosurface.h"

#define mod(a, b) (fmod((a), (b)) < 0 ? fmod((a), (b)) + (b) : fmod((a), (b)))

namespace mc { namespace samples { namespace screenshots {
  RepeatingIsosurface::RepeatingIsosurface(
      std::shared_ptr<ScalarField> sf,
      mcAlgorithmFlag algorithm,
      int xRepeat, int yRepeat,
      const glm::vec3 &position, const glm::quat &orientation)
    : MeshObject(position, orientation),
    m_sf(sf), m_xRepeat(xRepeat)
  {
    // Construct a repeating scalar field from our given scalar field
    RepeatingScalarField rsf(sf);
    // Generate the isosurface mesh
    IsosurfaceBuilder ib;
    auto mesh = ib.buildIsosurface(
        rsf,  // scalar field
        algorithm,  // algorithm
        4 * xRepeat, 4 * yRepeat, 8,  // resolution
        Vec3(0.0f, 0.0f, -2.0f),  // min
        Vec3((float)xRepeat, (float)yRepeat, 2.0f)  // max
        );
    // Send the mesh to the GL
    this->setMesh(*mesh);
  }

  RepeatingIsosurface::RepeatingScalarField::RepeatingScalarField(
      std::shared_ptr<ScalarField> sf)
    : m_sf(sf)
  {
  }

  float RepeatingIsosurface::RepeatingScalarField::operator()(
      float x, float y, float z) const
  {
    return (*m_sf)(mod(x, 1.0f), mod(y, 1.0f), z);
  }
} } }
