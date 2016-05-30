#include <glm/gtc/matrix_transform.hpp>

#include "arcballCamera.h"

namespace mc { namespace samples {

  ArcballCamera::ArcballCamera(float fovy, float near, float far,
      const glm::vec3 &position, const glm::quat &orientation,
      const glm::vec3 &followPoint)
    : PerspectiveCamera(fovy, near, far, position, orientation),
      m_followPoint (followPoint),
      m_arcballRadius(0.5f), m_dragStarted(false)
  {
  }

  void ArcballCamera::tick(float dt) {
  }

  glm::vec3 ArcballCamera::m_arcballPosition(float x, float y) {
    // Arcball control, from [Shoemake 1992]
    glm::vec3 pt;
    // Find the point on the sphere
    pt.x = (x - 0.5f) / m_arcballRadius;
    pt.y = (y - 0.5f) / m_arcballRadius;
    float r = pt.x * pt.x + pt.y * pt.y;
    if (r > 1.0) {
      // Correct for clicks outside of the sphere
      float s = 1.0f / sqrt(r);
      pt.x = s * pt.x;
      pt.y = s * pt.y;
      pt.z = 0.0f;
    } else {
      pt.z = sqrt(1.0f - r);
    }
    return pt;
  }

  bool ArcballCamera::mouseButtonEvent(
      const SDL_MouseButtonEvent &event,
      int windowWidth, int windowHeight)
  {
    if (event.button == SDL_BUTTON_LEFT) {
      switch (event.state) {
        case SDL_PRESSED:
          {
            m_dragStarted = true;
            // Store the initial position and orientation
            m_initialPosition = this->position();
            m_initialOrientation = this->orientation();
            // Normalize the click coordinates
            float x = float(event.x) / float(windowWidth);
            float y = float(windowHeight - event.y) / float(windowHeight);
            // Set the inital arcball position
            m_arcballInitial = m_arcballPosition(x, y);
            // Reset the final arcball position
            m_arcballFinal = m_arcballInitial;
          }
          break;
      }
      return true;
    }
    return false;
  }

  bool ArcballCamera::mouseMotionEvent(
      const SDL_MouseMotionEvent &event,
      int windowWidth, int windowHeight)
  {
    if (m_dragStarted && (event.state & SDL_BUTTON_LMASK)) {
      // Normalize the click coordinates
      float x = float(event.x) / float(windowWidth);
      float y = float(windowHeight - event.y) / float(windowHeight);
      // Update the final arcball position
      m_arcballFinal = m_arcballPosition(x, y);
      // Compute the rotation, from [Shoemake 1992]
      // We need to rotate around the object being tracked, so we translate the
      // object to the origin before rotating
      glm::mat4 rotation;
      rotation = glm::translate(rotation, m_followPoint);
      rotation *=
        glm::mat4_cast(
          glm::quat(
            glm::dot(m_arcballInitial, m_arcballFinal),
            glm::cross(m_arcballInitial, m_arcballFinal)));
      rotation = glm::translate(rotation, -m_followPoint);
      // Rotate the initial camera position around the follow point
      this->setPosition(
          glm::vec3(rotation * glm::vec4(m_initialPosition, 1.0f)));
      // Look at the follow point
      // TODO: Compute the look at orientation with quaternions
      glm::mat4 lookAt = glm::transpose(glm::lookAt(
          this->position(),
          m_followPoint,
          glm::vec3(0.0f, 0.0f, 1.0f)));
      this->setOrientation(glm::quat_cast(lookAt));
      return true;
    }
    return false;
  }

  void ArcballCamera::draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug) const
  {
    if (!debug)
      return;

    // TODO: Draw a sphere and the arc on the screen, for debugging purposes
  }
} }
