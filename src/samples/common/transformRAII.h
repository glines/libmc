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

#ifndef MC_SAMPLES_COMMON_TRANSFORM_RAII_H_
#define MC_SAMPLES_COMMON_TRANSFORM_RAII_H_

#include "transform.h"

namespace mc { namespace samples {
  /**
   * Implements a stack of matrix transforms that is unwound at the time of
   * destruction using the resource acquisition is initialization principle.
   */
  class TransformRAII : public Transform {
    private:
      Transform *m_base;
      size_t m_originalSize;
    public:
      /**
       * Construct a transform stack on top of the given stack which unwinds
       * automatically upon destruction.
       */
      TransformRAII(Transform &original);
      /**
       * Destroy the TransformRAII stack, which unwinds the transform stack
       * back to its original size.
       */
      ~TransformRAII();

      /**
       * Push the given matrix transform onto the top of the transform stack
       * and return the result. Pushing is performed as a matrix multiplication
       * on the top of the stack.
       *
       * \param matrix The matrix transform to push onto the transform stack.
       * \return The resulting value at the top of the transformation stack.
       *
       * \sa push()
       */
      const Transform &operator*=(const glm::mat4 &matrix);

      /**
       * Look at the current transform represented by the stack.
       */
      const glm::mat4 &peek() const { return m_base->peek(); }

      /**
       * Push the given matrix onto the transform stack. This is essentially a
       * matrix multiplication.
       *
       * \param matrix The matrix transform to push onto the transform stack.
       *
       * \sa operator*=()
       */
      void push(glm::mat4 matrix) { m_base->push(matrix); }

      /**
       * Remove the last matrix transform from the top of the transform stack.
       */
      void pop();
      /**
       * Returns the size of the transform stack.
       *
       * \return Current size of the transform stack.
       */
      size_t size() { return m_base->size(); }
    protected:
      Transform *getBase() { return m_base; }
      void unwind(size_t size);
  };
} }

#endif
