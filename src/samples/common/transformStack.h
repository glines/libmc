#ifndef MC_SAMPLES_COMMON_TRANSFORM_STACK_H_
#define MC_SAMPLES_COMMON_TRANSFORM_STACK_H_

#include <stack>

#include "transform.h"

namespace mc { namespace samples {
  /**
   * Stores a stack of matrix transforms.
   *
   * The TransformStack class is meant to be used in conjuction with the
   * TransformRAII class. TransformStack is the basis of all transforms, since
   * TransformRAII can only be instantiated from an existing instance of a
   * Transform class.
   */
  class TransformStack : public Transform {
    private:
      std::stack<glm::mat4> m_stack;
    public:
      TransformStack();
      ~TransformStack();

      const Transform &operator*=(const glm::mat4 &matrix);

      void push(glm::mat4 matrix);
      void pop();
      const glm::mat4 &peek() const {
        return this->m_stack.top();
      }
      size_t size() { return m_stack.size(); }

    protected:
      Transform *getBase() { return this; }
      void unwind(size_t size);
      void getIndex();
  };
} }

#endif
