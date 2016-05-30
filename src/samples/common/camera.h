#ifndef MC_SAMPLES_COMMON_CAMERA_H_
#define MC_SAMPLES_COMMON_CAMERA_H_

#include <glm/glm.hpp>

#include "sceneObject.h"

namespace mc { namespace samples {
  /**
   * Abstract class defining a camera in a 3D graphics scene.
   *
   * The camera can generate the worldView matrix transform from the camera's
   * current position and orientation and the frame interpolating value alpha.
   * Derived classes are to generate an appropriate projection matrix
   * transform.
   */
  class Camera : public SceneObject {
    public:
      Camera(const glm::vec3 &position, const glm::quat &orientation);
      virtual ~Camera();

      /**
       * Returns the transform from world space to view space based on the
       * camera's current position and orientation.
       *
       * The alpha value gives the interpolation weight between the last tick and
       * the current tick.
       */
      virtual glm::mat4 worldView(float alpha = 1.0) const;

      /**
       * Derived camera classes return a projection transform through this
       * method.
       *
       * The aspect value gives the aspect ratio of the viewport.
       *
       * The alpha value gives the interpolation weight between the last tick and
       * the current tick.
       */
      virtual glm::mat4 projection(float aspect, float alpha = 1.0f) const = 0;
  };
} }

#endif
