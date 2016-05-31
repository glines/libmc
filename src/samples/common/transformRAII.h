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
      TransformRAII(Transform &original);
      ~TransformRAII();

      const Transform &operator*=(const glm::mat4 &matrix);

      const glm::mat4 &peek() const { return m_base->peek(); }
      void push(glm::mat4 matrix) { m_base->push(matrix); }
      void pop();
      size_t size() { return m_base->size(); }
    protected:
      Transform *getBase() { return m_base; }
      void unwind(size_t size);
  };
} }

#endif
