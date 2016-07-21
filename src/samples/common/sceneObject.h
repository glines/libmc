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

#ifndef MC_SAMPLES_COMMON_SCENE_OBJECT_H_
#define MC_SAMPLES_COMMON_SCENE_OBJECT_H_

/**
 * \addtogroup samples
 * @{
 */

/**
 * \addtogroup common
 * @{
 */

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <unordered_map>

namespace mc { namespace samples {
  class Transform;
  /**
   * This abstract class defines a typical object in a 3D graphics scene.
   *
   * The SceneObject class has both position and orientation properties useful
   * for most rigid body objects. This class also stores the position and
   * orientation from the previous frame so that they may be interpolated with
   * the appropriate alpha value when it comes time to draw the scene object.
   */
  class SceneObject {
    friend class Scene;
    private:
      glm::vec3 m_position, m_prevPosition;
      glm::quat m_orientation, m_prevOrientation;

      std::unordered_map<
        const SceneObject *,
        std::shared_ptr<SceneObject>
        > m_children;

      /**
       * This method recursively advances the simulation of this object and all
       * of its children by dt seconds.
       */
      void m_tick(float dt);

      /**
       * This method recursively draws this object and all of its children.
       */
      void m_draw(Transform &modelWorld, const glm::mat4 &worldView,
          const glm::mat4 &projection, float alpha, bool debug);

    public:
      /**
       * Constructs a scene object with the given position and orientation.
       *
       * \param position The position of the scene object.
       * \param orientation The orientation of the scene object.
       */
      SceneObject(
          const glm::vec3 &position = glm::vec3(0.0f),
          const glm::quat &orientation = glm::quat());
      /**
       * Destroys this scene object. Since SceneObject is a polymorphic class,
       * this destructor is virtual.
       */
      virtual ~SceneObject();

      /**
       * Returns the position of this scene object. The \p alpha parameter
       * facilitates linear interpolation of the animation between keyframes of
       * an object's position for times in-between scene simulation ticks.
       *
       * \param alpha The weight between simulation keyframe ticks.
       * \return The weighted position of this scene object.
       */
      glm::vec3 position(float alpha = 1.0f) const {
        return glm::mix(m_prevPosition, m_position, alpha);
      }
      /**
       * Sets the position of this scene object for the current scene
       * simulation tick.
       *
       * \param position The position to set.
       */
      void setPosition(const glm::vec3 &position) {
        m_position = position;
      }

      /**
       * Returns the orientation of this scene object. The \p alpha parameter
       * facilitates SLERP interpolation of the animation between keyframes of
       * an object's orientation for times in-between scene simulation ticks.
       *
       * \param alpha The weight between simulation keyframe ticks.
       * \return The weighted orietation of this scene object.
       */
      glm::quat orientation(float alpha = 1.0f) const {
        return glm::slerp(m_prevOrientation, m_orientation, alpha);
      }
      /**
       * Sets the orientation of this scene object for the current scene
       * simulation tick.
       *
       * \param orientation The orientation to set.
       */
      void setOrientation(const glm::quat &orientation) {
        m_orientation = orientation;
      }

      /**
       * Adds a scene object as a child of this scene object.
       *
       * \param child Child scene object to add.
       *
       * \sa removeChild()
       */
      void addChild(std::shared_ptr<SceneObject> child) {
        m_children.insert({child.get(), child});
      }

      /**
       * \param The address of the child scene object to look for.
       * \return True if a scene object with the given address is a child of
       * this node, false otherwise.
       */
      bool hasChild(const SceneObject *address) {
        return m_children.find(address) != m_children.end();
      }

      /**
       * Removes the child of this scene object with the given address.
       *
       * \param address The memory address of the child to remove.
       *
       * \sa addChild()
       */
      void removeChild(const SceneObject *address);

      /**
       * Derived classes can implement this to receive an SDL event. All SDL
       * events are passed to each of the top-level scene objects in the scene.
       * Scene objects may choose to pass events to their children by calling
       * this method on their children. By returning true, implementing classes
       * can absorb the given event and mask it from handled elsewhere.
       *
       * \param event The current SDL event to be considered.
       * \return True if the given event was handled, and false otherwise.
       */
      virtual bool handleEvent(const SDL_Event &event) { return false; }

      /**
       * Advances the simulation of this scene object. Derived classes can
       * implement this method to give scene objects different behavior in
       * time.
       *
       * \param dt The delta time between the last tick and this current tick.
       */
      virtual void tick(float dt) {};

      /**
       * Draws this scene object in the scene. The default behavior of this
       * method is to draw nothing.
       *
       * \param modelWorld The model-space to world-space transform for this
       * scene object's position and orientation.
       * \param worldView The world-space to view-space transform for the
       * position and orientation of the camera currently being used.
       * \param projection The view-space to projection-space transform for
       * the camera that is currently being used.
       * \param alpha The simulation keyframe weight for animating this object
       * between keyframes.
       * \param debug Flag indicating whether or not debug information is to be
       * drawn.
       *
       * Derived classes must implement this method in order for their scene
       * objects to be visible.
       */
      virtual void draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug) {}
  };
} }

/** @} */

/** @} */

#endif

