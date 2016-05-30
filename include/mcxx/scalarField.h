#ifndef MCXX_SCALAR_FIELD_H_
#define MCXX_SCALAR_FIELD_H_

namespace mc {
  /**
   * An abstract class that serves as a functor equivalent for mcScalarField.
   */
  class ScalarField {
    public:
      ScalarField();

      virtual float operator()(float x, float y, float z) = 0;
  };
}

#endif
