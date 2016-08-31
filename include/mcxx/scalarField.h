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

#ifndef MCXX_SCALAR_FIELD_H_
#define MCXX_SCALAR_FIELD_H_

/**
 * \addtogroup libmcxx
 * @{
 */

extern "C" {
#include <mc/scalarField.h>
}

namespace mc {
  /**
   * An abstract class that serves as a functor equivalent for mcScalarField.
   */
  class ScalarField {
    private:
      mcScalarField m_sf;
    protected:
      ScalarField();
    public:
      /**
       * Constructs a scalar field functor from an ordinary scalar field
       * function. This constructor is useful when passing ordinary functions
       * to methods that expect a scalar field functor.
       *
       * \param sf The scalar field function pointer that defines the scalar
       * field we are constructing.
       */
      ScalarField(mcScalarField sf);
      virtual ~ScalarField();

      /**
       * Method that defines the scalar field function of this scalar field
       * functor. This method is implemented by ScalarField when constructed
       * with one of the public constructors.
       *
       * \param x The x-coordinate of the point to sample.
       * \param y The y-coordinate of the point to sample.
       * \param z The z-coordinate of the point to sample.
       * \return The value of the scalar field at the given sample point.
       *
       * Implementing classes must implement this method with their definition
       * of the scalar field function.
       */
      virtual float operator()(float x, float y, float z);
  };
}

/** @} */

#endif
