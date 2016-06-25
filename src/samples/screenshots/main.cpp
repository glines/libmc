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

/**
 * \defgroup screenshots screenshots
 * \brief Generates screenshots representitive of libmc
 *
 * Most of the screenshots of libmc that are included with libmc documentation
 * and other publications are generated with this program.
 *
 * The particular screenshot to render is chosen via command line arguments.
 * The screenshot scenes are rendered with OpenGL in a framebuffer whose pixels
 * are subsequently written to PNG image files.
 */

#include <GL/glew.h>
#include <SDL.h>
#include <cstdlib>
#include <getopt.h>
#include <glm/gtc/noise.hpp>
#include <png.h>

extern "C" {
#include <mc/algorithms.h>
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "../common/demo.h"
#include "../common/glError.h"
#include "../common/orthographicCamera.h"
#include "../common/scene.h"
#include "../common/template.h"
#include "repeatingIsosurface.h"

using namespace mc;
using namespace mc::samples;
using namespace mc::samples::screenshots;

/**
 * Class implementing the behavior of the screenshots sample program.
 */
class Screenshots : public Demo {
  public:
    /**
     * Constructs the screneshots sample program using the given command line
     * arguments.
     */
    Screenshots(int argc, char **argv)
      : Demo(argc, argv, "libmc Screenshot Sample")
    {
    }
};

Screenshots *demo;
std::shared_ptr<OrthographicCamera> camera;

void main_loop() {
  demo->mainLoop();
}

float sphere(float x, float y, float z) {
  static const float r = 0.6f;
  static const float a = 3.0f, b = 3.0f;
  return (x*x) / (a*a) + (y*y) / (b*b) + z*z - r*r;
}

float wave(float x, float y, float z) {
  float frequency = (float)M_PI * 4.0f;
  return z - (cos(x * frequency) * cos(y * frequency));
}

float noise(float x, float y, float z) {
  return glm::simplex(glm::vec3(x, y, z) * 5.0f);
}

float terrain(float x, float y, float z) {
  return z - glm::simplex(glm::vec2(x, y) * 6.0f);
}

int main(int argc, char **argv) {
  demo = new Screenshots(argc, argv);
  if (demo->argError()) {
    delete demo;
    return EXIT_FAILURE;
  }

  // Create the graphics scene
  static const int repeat = 1; 
  camera = std::shared_ptr<OrthographicCamera>(
      new OrthographicCamera(
        0.0f, (float)repeat, 0.0f, (float)repeat,  // left, right, bottom, top
        1.0f, 1000.0f,  // near, far
        glm::vec3(0.0f, -5.0f, 0.0f),  // position
        glm::angleAxis(
          (float)M_PI / 2.0f,
          glm::vec3(1.0f, 0.0f, 0.0f))  // orientation
        ));
  demo->scene()->addObject(camera);
  demo->setCamera(camera);

  // TODO: Set up our scene so that the image we render will tessellate nicely
  // in the background of the documentation. We do this by orienting the ortho
  // camera parallel to one of the sample grid axis, and then providing the
  // isosurface extraction algorithm a scalar field whose values wrap precicely
  // at the boundries of our view volume. Any triangles generated near one side
  // of the view volume boundry will be computed exactly the same as triangles
  // on the other side, because the scalar field values will be exactly the
  // same.
  auto sf = std::shared_ptr<ScalarField>(
      new ScalarField(terrain));
  auto repeatingIsosurface = std::shared_ptr<RepeatingIsosurface>(
      new RepeatingIsosurface(sf,
        demo->algorithm(),  // algorithm
        repeat, repeat,  // xRepeat, yRepeat
        glm::vec3(0.0f, 0.0f, 0.0f),  // position
        glm::angleAxis(
          (float)M_PI / 2.0f,
          glm::vec3(1.0f, 0.0f, 0.0f))  // orientation
        ));
  repeatingIsosurface->setDrawOpaque(true);
  repeatingIsosurface->setDrawWireframe(false);
  demo->scene()->addObject(repeatingIsosurface);

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

  delete demo;

  return EXIT_SUCCESS;
}
