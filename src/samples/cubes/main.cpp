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

#include <GL/glew.h>
#include <SDL.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <glm/gtc/quaternion.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

extern "C" {
#include <mc/algorithms/common/cube.h>
}

#include "../common/arcballCamera.h"
#include "../common/demo.h"
#include "../common/scene.h"
#include "cubeObject.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

using namespace mc::samples;
using namespace mc::samples::cubes;

/**
 * Implements the 256 voxel cubes demo.
 */
class Cubes : public Demo {
  private:
    std::shared_ptr<ArcballCamera> m_camera;
    std::shared_ptr<CubeObject> m_cubeObject;
    int m_res;
  public:
    /**
     * Construct the main class for the 256 voxel cubes demo with the given
     * arguments.
     *
     * \param argc Number of command line arguments, including the command
     * image name.
     * \param argv Array of command line arguments.
     */
    Cubes(int argc, char **argv)
      : Demo(argc, argv, "256 Voxel Cubes"), m_res(8)
    {
      if (this->argError())
        return;

      // Populate the graphics scene
      m_camera = std::shared_ptr<ArcballCamera>(
          new ArcballCamera(
            90.0f,  // fovy
            0.1f,  // near
            1000.0f,  // far
            glm::vec3(0.0f, -2.0f, 0.0f),  // position
            glm::angleAxis(
              (float)M_PI / 2.0f,
              glm::vec3(1.0f, 0.0f, 0.0f)),  // orientation
            glm::vec3(0.0f, 0.0f, 0.0f)  // followPoint
            ));
      this->scene()->addObject(m_camera);
      this->setCamera(m_camera);
      m_cubeObject = std::shared_ptr<CubeObject>(
          new CubeObject(0x01, m_res, m_res, m_res));
      this->scene()->addObject(m_cubeObject);

      if (this->sceneString() != nullptr) {
        fprintf(stderr, "sceneString: %s\n", this->sceneString());
        if (!setSceneString(this->sceneString())) {
          // TODO: Pass this error to the context constructing this class
        }
      }
    }

    /**
     * Sets the resolution of the sample lattice used to extract the
     * isosurface. With higher resolutions, the interpolated isosurface has
     * more polygons.
     *
     * \param res The sample lattice resolution to use. The resolution must be
     * a positive value.
     */
    void setResolution(int res) {
      assert(res > 0);
      m_res = res;
      m_cubeObject->setResolution(res, res, res);
    }

    /**
     * Configures the cube scene according to the given string. The string is
     * encoded in a format that allows one to specify the camera angle, cube
     * configuration, and other parameters of the cube to be drawn.
     *
     * \param scene String representing the camera angle, cube configuration,
     * etc.
     *
     * camera=topLeft,cube=0x4e,res=8
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
        if (strcmp(ident, "algorithm") == 0) {
          // Set the isosurface extraction algorithm
          auto algorithm = mcAlgorithm_stringToFlag(value);
          if (algorithm == MC_UNKNOWN_ALGORITHM) {
            fprintf(stderr, "Error: Unknown algorithm '%s'\n", value);
            return false;
          }
          m_cubeObject->setAlgorithm(algorithm);
        } else if (strcmp(ident, "camera") == 0) {
          if (strcmp(value, "topLeft") == 0) {
            // Set the camera looking down on the top left corner of the cube
            auto camera = std::shared_ptr<Camera>(
                new PerspectiveCamera(
                  (float)M_PI / 10.0f,  // fovy
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
        } else if (strcmp(ident, "cube") == 0) {
          int cube = strtol(value, nullptr, 16);
          if (cube >= 0 && cube <= 0xff) {
            m_cubeObject->setCube(cube);
          } else {
            fprintf(stderr, "Invalid value '0x%x' for cube configuration\n",
                cube);
            return false;
          }
        } else if (strcmp(ident, "res") == 0) {
          int res = strtol(value, nullptr, 10);
          if (res > 0) {
            this->setResolution(res);
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
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
          m_camera->mouseButtonEvent(event.button,
              this->width(), this->height());
          return true;
        case SDL_MOUSEMOTION:
          m_camera->mouseMotionEvent(event.motion,
              this->width(), this->height());
          return true;
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case SDLK_m:
              m_cubeObject->setAlgorithm(MC_SIMPLE_MARCHING_CUBES);
              return true;
            case SDLK_d:
              m_cubeObject->setAlgorithm(MC_DUAL_MARCHING_CUBES);
              return true;
            case SDLK_s:
              m_cubeObject->setAlgorithm(MC_ELASTIC_SURFACE_NETS);
              return true;
            case SDLK_c:
              m_cubeObject->setAlgorithm(MC_CUBERILLE);
              return true;
            case SDLK_p:
              m_cubeObject->setAlgorithm(MC_PATCH_MARCHING_CUBES);
              return true;
            case SDLK_u:
              m_cubeObject->setAlgorithm(MC_NIELSON_DUAL);
              return true;
#define INTENSITY_DELTA 0.1f
            case SDLK_COMMA:
              m_cubeObject->setIntensity(
                  m_cubeObject->intensity() - INTENSITY_DELTA);
              fprintf(stderr, "intensity: %g\n", m_cubeObject->intensity());
              return true;
            case SDLK_PERIOD:
              m_cubeObject->setIntensity(
                  m_cubeObject->intensity() + INTENSITY_DELTA);
              fprintf(stderr, "intensity: %g\n", m_cubeObject->intensity());
              return true;
            case SDLK_w:
              m_cubeObject->setDrawWireframe(!m_cubeObject->isDrawWireframe());
              return true;
            case SDLK_n:
              m_cubeObject->setDrawNormals(!m_cubeObject->isDrawNormals());
              return true;
            case SDLK_o:
              m_cubeObject->setDrawOpaque(!m_cubeObject->isDrawOpaque());
              return true;
          }
          switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:
              this->setResolution(min(max(m_res * 2, 2), 64));
              return true;
            case SDL_SCANCODE_DOWN:
              this->setResolution(min(max(m_res / 2, 2), 64));
              return true;
            case SDL_SCANCODE_RIGHT:
              m_cubeObject->setCube((m_cubeObject->cube() + 1) % 256);
              return true;
            case SDL_SCANCODE_LEFT:
              m_cubeObject->setCube((m_cubeObject->cube() - 1) % 256);
              return true;
            case SDL_SCANCODE_PAGEUP:
              if (SDL_GetModState() & KMOD_SHIFT) {
                for (int i = MC_CUBE_NUM_CANONICAL_ORIENTATION_INVERSIONS - 1; i >= 0; --i) {
                  if (mcCube_canonicalOrientationInversions[i] < m_cubeObject->cube()) {
                    fprintf(stdout, "mcCube_canonicalOrientationInversions[%d]: 0x%02x\n",
                        i, mcCube_canonicalOrientationInversions[i]);
                    m_cubeObject->setCube(
                        mcCube_canonicalOrientationInversions[i]);
                    return true;
                  }
                }
              } else {
                for (int i = MC_CUBE_NUM_CANONICAL_ORIENTATIONS - 1; i >= 0; --i) {
                  if (mcCube_canonicalOrientations[i] < m_cubeObject->cube()) {
                    fprintf(stdout, "mcCube_canonicalOrientations[%d]: 0x%02x\n",
                        i, mcCube_canonicalOrientations[i]);
                    m_cubeObject->setCube(mcCube_canonicalOrientations[i]);
                    return true;
                  }
                }
              }
              break;
            case SDL_SCANCODE_PAGEDOWN:
              if (SDL_GetModState() & KMOD_SHIFT) {
                for (int i = 0; i < MC_CUBE_NUM_CANONICAL_ORIENTATION_INVERSIONS; ++i) {
                  if (mcCube_canonicalOrientationInversions[i] > m_cubeObject->cube()) {
                    fprintf(stdout, "mcCube_canonicalOrientationInversions[%d]: 0x%02x\n",
                        i, mcCube_canonicalOrientationInversions[i]);
                    m_cubeObject->setCube(
                        mcCube_canonicalOrientationInversions[i]);
                    return true;
                  }
                }
              } else {
                for (int i = 0; i < MC_CUBE_NUM_CANONICAL_ORIENTATIONS; ++i) {
                  if (mcCube_canonicalOrientations[i] > m_cubeObject->cube()) {
                    fprintf(stdout, "mcCube_canonicalOrientations[%d]: 0x%02x\n",
                        i, mcCube_canonicalOrientations[i]);
                    m_cubeObject->setCube(mcCube_canonicalOrientations[i]);
                    return true;
                  }
                }
              }
              break;
            case SDL_SCANCODE_SPACE:
              m_cubeObject->setDrawScalarField(
                  !m_cubeObject->isDrawScalarField());
              return true;
            default:
              ;
          }
          break;
      }
      return false;
    }
};

Cubes *demo;

void main_loop() {
  demo->mainLoop();
}

int main(int argc, char **argv) {
  demo = new Cubes(argc, argv);
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
