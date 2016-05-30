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
