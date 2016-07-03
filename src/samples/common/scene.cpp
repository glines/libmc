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

  void Scene::removeObject(const SceneObject *address) {
    auto iterator = m_objects.find(address);
    assert(iterator != m_objects.end());
    // FIXME: Remove this node from its parent
//    iterator->parent()->removeChild(address);
    m_objects.erase(address);
  }

  bool Scene::handleEvent(const SDL_Event &event) {
    for (auto object : m_objects) {
      if (object.second->handleEvent(event))
        return true;
    }
    return false;
  }

  void Scene::tick(float dt) {
    // Advance the simulation on all top-level scene objects
    for (auto object : this->m_objects) {
      object.second->m_tick(dt);
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
      object.second->m_draw(
          modelWorld,
          worldView,
          projection,
          alpha,
          debug);
    }
  }
} }
