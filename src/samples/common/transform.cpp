#include <glm/gtx/string_cast.hpp>

#include "transform.h"

namespace mc { namespace samples {
  Transform::Transform() {
  }

  Transform::~Transform() {
  }

  std::string Transform::toString() {
    return glm::to_string(this->peek());
  }
} }
