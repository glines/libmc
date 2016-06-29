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

#ifndef MC_SAMPLES_COMMON_SCENE_H_
#define MC_SAMPLES_COMMON_SCENE_H_

#include <SDL.h>
#include <memory>
#include <vector>

namespace mc { namespace samples {
  class Camera;
  class SceneObject;
  /**
   * This class implements a simple graphics scene.
   *
   * OpenGL is typically used under the hood, but nothing about this scene
   * class requires the use of any specific graphics library.
   */
  class Scene {
    private:
      std::vector<std::shared_ptr<SceneObject>> m_objects;

    public:
      /**
       * Constructs a graphics scene. The default scene is empty.
       */
      Scene();
      /**
       * Destroys this graphics scene. All shared pointers to scene objects
       * held by the scene are released.
       */
      ~Scene();

      /**
       * Adds the given scene object to the top level of this graphics scene.
       */
      void addObject(std::shared_ptr<SceneObject> object) {
        this->m_objects.push_back(object);
      }

      /**
       * \return A reference to the array of top-level objects in this graphics
       * scene.
       */
      std::vector<std::shared_ptr<SceneObject>> &sceneObjects() {
        return this->m_objects;
      }

      /**
       * Allow objects in the scene to handle the given event. All SDL
       * events are passed to each of the top-level scene objects in the scene.
       * If a scene object decides to handle the given event, this method
       * returns true.
       *
       * \param event The SDL event to be considered by objects in the scene.
       * \return True if the given event was handled by a scene object, false
       * otherwise.
       */
      bool handleEvent(const SDL_Event &event);

      /**
       * Advances the simulation of the scene (and the simulations carried out
       * by each scene object) forward by the given time interval in seconds.
       */
      void tick(float dt);

      /**
       * Draws the scene by recursively drawing all of its scene objects.
       *
       * \param camera The camera that dictates the world-view and projection
       * transforms to use when drawing the scene.
       * \param aspect The aspect ratio of the viewport being drawn. Passing
       * this value ensures that the aspect ratio will be preserved by the
       * projection transform.
       * \param alpha A keyframe weight that controls interpolation between
       * ticks.  Scene objects should strive to interpolate between the values
       * of the last tick and the current tick wherever possible.
       * \param debug A hint given to scene objects so that they can
       * toggle the drawing of debug information.
       *
       * Since the scene class makes no provisions for supporting any
       * particular graphics API, it is the responsibility of the derived scene
       * objects to draw using the correct API.
       */
      void draw(const Camera &camera, float aspect,
          float alpha = 1.0, bool debug = false) const;
  };
} }

#endif
