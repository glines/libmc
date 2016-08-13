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

#include <mcxx/contour.h>
#include <mcxx/scalarField.h>
#include <mcxx/vector.h>

#include <mcxx/contourBuilder.h>

namespace mc {
  ContourBuilder::ContourBuilder() {
    mcContourBuilder_init(&m_internal);
  }

  ContourBuilder::~ContourBuilder() {
    for (auto contour : m_contours) {
      delete contour;
    }
    mcContourBuilder_destroy(&m_internal);
  }

  float wrapScalarField(float x, float y, float z, ScalarField *sf) {
    return (*sf)(x, y, z);
  }

  const Contour *ContourBuilder::buildContour(
      ScalarField &sf,
      mcAlgorithmFlag algorithm,
      int x_res, int y_res,
      const Vec2 &min, const Vec2 &max)
  {
    // Pass the scalar field functor as an argument
    const mcContour *c = mcContourBuilder_contourFromFieldWithArgs(
        &m_internal,
        (mcScalarFieldWithArgs)wrapScalarField,
         &sf,
         algorithm,
         x_res, y_res,
         &min.to_mcVec2(), &max.to_mcVec2()
         );
    auto contour = new Contour(c);
    m_contours.push_back(contour);
    return contour;
  }
}
