#include "camera.h"
#include "sceneObject.h"
#include "transformStack.h"

#include "scene.h"

namespace mc { namespace samples {
  Scene::Scene() {
    // TODO
  }

  Scene::~Scene() {
    // TODO
  }

  void Scene::tick(float dt) {
    // Advance the simulation on all top-level scene objects
    for (auto object : this->m_objects) {
      object->m_tick(dt);
    }
  }

  void Scene::draw(const Camera &camera, float aspect,
      float alpha, bool debug) const
  {
    // Start with an empty modelWorld transform stack
    TransformStack modelWorld;

    // Obtain transforms from the camera
    auto worldView = camera.worldView(alpha);
    auto projection = camera.projection(aspect, alpha);

    // TODO: Draw the skybox first

    // Iterate through all top-level scene objects and draw them
    for (auto object : this->m_objects) {
      object->m_draw(
          modelWorld,
          worldView,
          projection,
          alpha,
          debug);
    }
  }
} }
