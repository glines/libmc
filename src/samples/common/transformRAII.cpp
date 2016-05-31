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

#include "transformRAII.h"

namespace mc { namespace samples {
  TransformRAII::TransformRAII(Transform &original) {
    m_base = original.getBase();
    m_originalSize = m_base->size();
  }

  TransformRAII::~TransformRAII() {
    // Unwind the transformation back to what it was when we were constructed
    m_base->unwind(m_originalSize);
    assert(m_base->size() == m_originalSize);
  }

  const Transform &TransformRAII::operator*=(const glm::mat4 &matrix) {
    this->push(matrix);
    return *this;
  }

  void TransformRAII::pop() {
    // We shouldn't pop past the original size
    assert(m_base->size() > m_originalSize);

    m_base->pop();
  }

  void TransformRAII::unwind(size_t size) {
    // We shouldn't unwind past the original size
    assert(size >= m_originalSize);

    m_base->unwind(size);
  }
} }
