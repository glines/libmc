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
