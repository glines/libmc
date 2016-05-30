#include <glm/gtc/matrix_transform.hpp>

#include "transformRAII.h"

#include "sceneObject.h"

namespace mc { namespace samples {
  SceneObject::SceneObject(
      const glm::vec3 &position, const glm::quat &orientation)
    : m_position(position), m_prevPosition(position),
      m_orientation(orientation), m_prevOrientation(orientation)
  {
  }

  SceneObject::~SceneObject() {
  }
  void SceneObject::m_tick(float dt) {
    // Delegate the actual simulation to derived classes
    this->tick(dt);

    // Tick all of our children
    for (auto child : this->m_children) {
      child->m_tick(dt);
    }
  }

  void SceneObject::m_draw(Transform &modelWorld, const glm::mat4 &worldView,
      const glm::mat4 &projection, float alpha, bool debug) const
  {
    TransformRAII mw(modelWorld);
    // Translate the object into position
    mw *= glm::translate(glm::mat4(), this->position(alpha));
    // Apply the object orientation as a rotation
    mw *= glm::mat4_cast(this->orientation(alpha));

    // Delegate the actual drawing to derived classes
    this->draw(mw.peek(), worldView, projection, alpha, debug);

    // Draw our children
    for (auto child : m_children) {
      child->m_draw(mw, worldView, projection, alpha, debug);
    }
  }
} }
