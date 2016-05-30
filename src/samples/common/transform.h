#ifndef SANDBOX_COMMON_TRANSFORM_H_
#define SANDBOX_COMMON_TRANSFORM_H_

#include <string>

#include <glm/glm.hpp>

namespace mc { namespace samples {
  class TransformRAII;

  /**
   * The Transform class defines an interface for applying matrix
   * transformations with a stack discipline.
   *
   * Transform is a pure virtual class that describes the interface shared by
   * TransformStack and TransformRAII. The former is used to store transforms,
   * while the latter is used to ease the burden of managing the transform
   * stack.
   */
  class Transform {
    // Friend declaration needed in order to share base transform among all
    // TransformRAII objects
    friend TransformRAII;
    public:
      Transform();
      virtual ~Transform();

      /**
       * Performs matrix multiplication with the top of the stack and the given
       * matrix, and places the result as the new top of the transformation
       * stack.
       */
      virtual const Transform &operator*=(const glm::mat4 &matrix) = 0;

      /**
       * Performs matrix multiplication with the top of the stack and the given
       * matrix, and places the result as the new top of the transformation
       * stack.
       */
      virtual void push(glm::mat4 matrix) = 0;

      /**
       * Returns the topmost transform of the transformation stack without
       * removing it.
       */
      virtual const glm::mat4 &peek() const = 0;

      /**
       * Removes the topmost element of the transformation stack.
       */
      virtual void pop() = 0;

      /**
       * Returns the number of transforms that have been pushed onto the
       * transformation stack.
       */
      virtual size_t size() = 0;

      /**
       * Returns a string representing the topmost transform on the stack, for
       * debugging purposes.
       */
      std::string toString();

    protected:
      virtual Transform *getBase() = 0;
      virtual void unwind(size_t index) = 0;
  };
} }

#endif
