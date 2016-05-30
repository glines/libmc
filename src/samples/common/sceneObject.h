#ifndef MC_SAMPLES_COMMON_SCENE_OBJECT_H_
#define MC_SAMPLES_COMMON_SCENE_OBJECT_H_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <vector>

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

      std::vector<std::shared_ptr<SceneObject>> m_children;

      /**
       * This method recursively advances the simulation of this object and all
       * of its children by dt seconds.
       */
      void m_tick(float dt);

      /**
       * This method recursively draws this object and all of its children.
       */
      void m_draw(Transform &modelWorld, const glm::mat4 &worldView,
          const glm::mat4 &projection, float alpha, bool debug) const;

    public:
      SceneObject(const glm::vec3 &position, const glm::quat &orientation);
      virtual ~SceneObject();

      glm::vec3 position(float alpha = 1.0f) const {
        return glm::mix(m_prevPosition, m_position, alpha);
      }
      void setPosition(const glm::vec3 &position) {
        m_position = position;
      }

      glm::quat orientation(float alpha = 1.0f) const {
        return glm::slerp(m_prevOrientation, m_orientation, alpha);
      }
      void setOrientation(const glm::quat &orientation) {
        m_orientation = orientation;
      }

      void addChild(std::shared_ptr<SceneObject> child) {
        m_children.push_back(child);
      }

      virtual void tick(float dt) {};

      virtual void draw(const glm::mat4 &modelWorld,
          const glm::mat4 &worldView, const glm::mat4 &projection,
          float alpha, bool debug) const {}
  };
} }

#endif

