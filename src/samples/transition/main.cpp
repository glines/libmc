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

#include <cstdlib>
#include <mcxx/vector.h>

extern "C" {
#include <mc/algorithms/transvoxel.h>
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "../common/demo.h"
#include "../common/isosurfaceObject.h"
#include "../common/scene.h"
#include "../common/wasdCamera.h"
#include "interpolatingOctree.h"
#include "transitionObject.h"

using namespace mc::samples;
using namespace mc::samples::transition;

class TransitionDemo : public Demo {
  private:
    std::shared_ptr<WasdCamera> m_camera;
    std::shared_ptr<InterpolatingOctree> m_octree;
    std::shared_ptr<IsosurfaceObject> m_isosurface;
    std::shared_ptr<TransitionObject> m_transitionObject;
  public:
    TransitionDemo(int argc, char **argv)
      : Demo(argc, argv, "Transition Voxel Demo")
    {
      if (this->argError())
        return;

      // Populate the graphics scene
      m_camera = std::shared_ptr<WasdCamera>(
          new WasdCamera(
            80.0f * ((float)M_PI / 180.0f),  // fovy
            0.1f,  // near
            100000.0f,  // far
            glm::vec3(0.0f, -5.0f, 0.0f),  // position
            glm::angleAxis(
              (float)M_PI / 2.0f,
              glm::vec3(1.0f, 0.0f, 0.0f))  // orientation
            ));
      this->scene()->addObject(m_camera);
      this->setCamera(m_camera);
      /*
      m_octree = std::shared_ptr<InterpolatingOctree>(
          new InterpolatingOctree());
      this->scene()->addObject(m_octree);
      // XXX: Test the octree
      OctreeCoordinates pos;
      */
      /*
      pos.x = 0;
      pos.y = 0;
      pos.z = 0;
      m_octree->getNode(pos, 0);
      pos.x = 1;
      pos.y = 1;
      pos.z = 1;
      m_octree->getNode(pos, 0);
      pos.x = 2;
      pos.y = 2;
      pos.z = 2;
      m_octree->getNode(pos, 0);
      */
      /*
      pos.x = -1;
      pos.y = -1;
      pos.z = -1;
      m_octree->setSample(pos, -1.0f);
      */
      /*
      for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
          pos.x = x;
          pos.y = y;
          pos.z = 0;
          m_octree->setSample(pos, -1.0f);
        }
      }
      m_isosurface = std::shared_ptr<IsosurfaceObject>(
          new IsosurfaceObject());
      m_isosurface->setScalarField(m_octree);
      mc::Vec3 min(-2.0f, -2.0f, -2.0f);
      m_isosurface->setMin(min);
      mc::Vec3 max(2.0f, 2.0f, 2.0f);
      m_isosurface->setMax(max);
      m_isosurface->setPosition(glm::vec3(-2.0f, -2.0f, -2.0f));
      this->scene()->addObject(m_isosurface);
      */
      // Test the transition object
      m_transitionObject = std::shared_ptr<TransitionObject>(
          new TransitionObject(
            0x01,  // transition cell configuration
            glm::vec3(-0.5f, -0.5f, -0.5f)  // position
            ));
      this->scene()->addObject(m_transitionObject);

      // Parse the scene string
      if (this->sceneString() != nullptr) {
        fprintf(stderr, "sceneString: %s\n", this->sceneString());
        if (!setSceneString(this->sceneString())) {
          // TODO: Pass this error to the context constructing this class
        }
      }
    }

    /**
     * Configures the transition cell scene according to the given string. The
     * string is encoded in a format that allows one to specify the camera
     * angle, transition cell configuration, and other parameters of the
     * transition cell to be drawn.
     *
     * \param scene String representing the camera angle, cell configuration,
     * etc.
     * \return True if the scene string passed was valid, false otherwise.
     *
     * camera=topLeft,cell=0x04e,res=2
     */
    bool setSceneString(const char *scene) {
      std::shared_ptr<char> copy;
      int length;
      length = strlen(scene) + 1;
      fprintf(stderr, "scene string: %s\n", scene);
      copy = std::shared_ptr<char>(new char[length]);
      memcpy(copy.get(), scene, sizeof(char) * length);
      // Tokenify the string
      char *token, *state;
      // FIXME: strtok_r() might not be portable C99
      token = strtok_r(copy.get(), ",", &state);
      while (token != nullptr) {
        // Split the token into identifier and value
        char *ident, *value, *state2;
        ident = strtok_r(token, "=", &state2);
        value = strtok_r(nullptr, "=", &state2);
        if (strcmp(ident, "camera") == 0) {
          if (strcmp(value, "topLeft") == 0) {
            // Set the camera looking down on the top left corner of the cube
            auto camera = std::shared_ptr<Camera>(
                new PerspectiveCamera(
                  9.0f * ((float)M_PI / 180.0f),  // fovy
                  0.1f,  // near
                  1000.0f,  // far
                  glm::vec3(7.48113f, -6.50764f, 5.34367f),  // position
                  glm::quat(0.783f, 0.461f, 0.217f, 0.356f)  // orientation
                  ));
            this->scene()->addObject(camera);
            this->setCamera(camera);
          } else {
            fprintf(stderr, "Error: Unknown camera angle '%s'\n",
                value);
            return false;
          }
        } else if (strcmp(ident, "cell") == 0) {
          int cell = strtol(value, nullptr, 16);
          if (cell >= 0 && cell <= 0x1ff) {
            m_transitionObject->setCell(cell);
          } else {
            fprintf(stderr, "Invalid value '0x%x' for cell configuration\n",
                cell);
            return false;
          }
        } else if (strcmp(ident, "res") == 0) {
          int res = strtol(value, nullptr, 10);
          if (res > 0) {
            // TODO
//            this->setResolution(res);
          } else {
            fprintf(stderr, "Invalid value '%x' for sample lattice resolution\n",
                res);
            return false;
          }
        } else {
          fprintf(stderr, "Unknown identifier '%s'\n", ident);
          return false;
        }
        // Move on to the next token
        token = strtok_r(nullptr, ",", &state);
      }
      return true;
    }

    bool handleEvent(const SDL_Event &event) {
      switch (event.type) {
        case SDL_KEYDOWN:
          switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_PAGEUP:
              if (SDL_GetModState() & KMOD_SHIFT) {
                for (
                    int i = MC_TRANSVOXEL_NUM_CANONICAL_TRANSITION_CELLS - 1;
                    i >= 0;
                    --i)
                {
                  if (mcTransvoxel_canonicalTransitionCells[i] < m_transitionObject->cell()) {
                    fprintf(stdout, "mcTransvoxel_canonicalTransitionCells[%d]: 0x%03x\n",
                        i, mcTransvoxel_canonicalTransitionCells[i]);
                    m_transitionObject->setCell(
                        mcTransvoxel_canonicalTransitionCells[i]);
                    break;
                  }
                }
              } else {
                m_transitionObject->setCell(
                    (m_transitionObject->cell() - 1) % 512);
              }
              break;
            case SDL_SCANCODE_PAGEDOWN:
              if (SDL_GetModState() & KMOD_SHIFT) {
                for (
                    int i = 0;
                    i < MC_TRANSVOXEL_NUM_CANONICAL_TRANSITION_CELLS;
                    ++i)
                {
                  if (mcTransvoxel_canonicalTransitionCells[i] > m_transitionObject->cell()) {
                    fprintf(stdout, "mcTransvoxel_canonicalTransitionCells[%d]: 0x%03x\n",
                        i, mcTransvoxel_canonicalTransitionCells[i]);
                    m_transitionObject->setCell(
                        mcTransvoxel_canonicalTransitionCells[i]);
                    break;
                  }
                }
              } else {
                m_transitionObject->setCell(
                    (m_transitionObject->cell() + 1) % 512);
              }
              break;
          }
          break;
      }
      return false;
    }
};

TransitionDemo *demo;

void main_loop() {
  demo->mainLoop();
}

int main(int argc, char **argv) {
  demo = new TransitionDemo(argc, argv);
  if (demo->argError()) {
    delete demo;
    return EXIT_FAILURE;
  }

  if (demo->isScreenshot()) {
    demo->drawScreenshot();
  } else {
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (1) {
      main_loop();
      // TODO: Wait for VSync? Or should we poll input faster than that?
    }
#endif
  }

  // FIXME: This can never be reached if the demo object calls exit()
  delete demo;

  return EXIT_SUCCESS;
}
