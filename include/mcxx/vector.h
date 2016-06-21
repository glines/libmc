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

#ifndef MCXX_VECTOR_H_
#define MCXX_VECTOR_H_

extern "C" {
#include <mc/vector.h>
}

namespace mc {
  /**
   * Floating point representation for a vector in \f$\mathbb{R}^3\f$. This is
   * a wrapper around the mcVec3 structure and its associated routines.
   *
   * \sa mcVec3
   */
  class Vec3 {
    private:
      mcVec3 m_internal;
    public:
      /**
       * Constructs a vector in \f$\mathbb{R}^3\f$ with the given coordinates.
       *
       * \param x The x-coordinate of the vector.
       * \param y The y-coordinate of the vector.
       * \param z The z-coordinate of the vector.
       */
      Vec3(float x, float y, float z);

      /**
       * Returns the mcVec3 struct representation of this vector, as expected
       * by the C API of libmc.
       *
       * \return The mcVec3 representation of this vector.
       */
      const mcVec3 &to_mcVec3() const { return m_internal; }
  };
}

#endif
