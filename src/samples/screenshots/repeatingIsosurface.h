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

#ifndef MC_SAMLPES_SCREENSHOTS_REPEATING_ISOSURFACE_H_
#define MC_SAMLPES_SCREENSHOTS_REPEATING_ISOSURFACE_H_

/**
 * \addtogroup samples
 * @{
 */

/**
 * \addtogroup screenshots
 * @{
 */

#include <memory>

#include <mcxx/scalarField.h>

#include "../common/meshObject.h"

namespace mc { namespace samples { namespace screenshots {
  /**
   * This renderable scene object represents an isosurface whose underlying
   * scalar field is repeated with respect to the xy-plane. This allows us to
   * generate tessellating triangulations with most of the isosurface
   * extraction algorithms in libmc, which can be used for artistic
   * embellishment in the background of the libmc documentation.
   *
   * Note that some scalar fields, such as ones based on trig functions, are
   * naturally repeating and do not necessarily need to be forcibly repeated in
   * this way.
   */
  class RepeatingIsosurface : public MeshObject {
    private:
      std::shared_ptr<ScalarField> m_sf;
      int m_xRepeat, m_yRepeat;

      class RepeatingScalarField : public ScalarField {
        private:
          std::shared_ptr<ScalarField> m_sf;
        public:
          RepeatingScalarField(std::shared_ptr<ScalarField> sf);

          float operator()(float x, float y, float z) const;
      };
    public:
      /**
       * Constructs a repeating isosurface that uses the given scalar field for
       * the implicit definition of the isosurface.
       *
       * The scalar field function is only sampled between 0.0f and 1.0f on all
       * axes, and repeated the given number of times with respect to the
       * xy-plane. The resulting mesh starts at the origin and ends at
       * \code{.c}
       * x = (float)x_repeat * 1.0f;
       * y = (float)y_repeat * 1.0f;
       * z = max(x, y);
       * \endcode
       */
      RepeatingIsosurface(
          std::shared_ptr<ScalarField> sf,
          int xRepeat = 3, int yRepeat = 3,
          const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
          const glm::quat &orientation = glm::quat());
  };
} } }

/** @} */

/** @} */

#endif
