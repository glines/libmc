#include "transformStack.h"

namespace mc { namespace samples {
  TransformStack::TransformStack() {
    m_stack.push(glm::mat4());
  }

  TransformStack::~TransformStack() {
  }

  const Transform &TransformStack::operator*=(const glm::mat4 &matrix) {
    this->push(matrix);
    return *this;
  }

  void TransformStack::push(glm::mat4 matrix) {
    this->m_stack.push(
        this->m_stack.top() * matrix);
  }

  void TransformStack::pop() {
    this->m_stack.pop();
  }

  void TransformStack::unwind(size_t size) {
    while (m_stack.size() > size)
      m_stack.pop();
  }
} }
