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
#include <glm/gtc/noise.hpp>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "../common/orthographicCamera.h"
#include "../common/scene.h"
#include "repeatingIsosurface.h"

using namespace mc;
using namespace mc::samples;
using namespace mc::samples::screenshots;

struct demo {
  SDL_Window *window;
  SDL_GLContext glContext;
  int window_width, window_height;
  Scene *scene;
  std::shared_ptr<OrthographicCamera> camera;
  std::shared_ptr<RepeatingIsosurface> repeatingIsosurface;
} demo;

void init_sdl() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Failed to initialize SDL: %s\n",
        SDL_GetError());
    exit(EXIT_FAILURE);
  }
  demo.window_width = 640;
  demo.window_height = 480;
  demo.window = SDL_CreateWindow(
      "libmc Screenshots",  // title
      SDL_WINDOWPOS_UNDEFINED,  // x
      SDL_WINDOWPOS_UNDEFINED,  // y
      demo.window_width, demo.window_height,  // w, h
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE  // flags
      );
  if (demo.window == NULL) {
    fprintf(stderr, "Failed to create SDL window: %s\n",
        SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

void init_gl() {
  // Create an OpenGL context for our window
  demo.glContext = SDL_GL_CreateContext(demo.window);
  if (demo.glContext == NULL) {
    fprintf(stderr, "Failed to initialize OpenGL context: %s\n",
        SDL_GetError());
    exit(EXIT_FAILURE);
  }

  // Initialize GL entry points
  GLenum error = glewInit();
  if (error != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW: %s\n",
        glewGetErrorString(error));
    exit(EXIT_FAILURE);
  }

  // Configure the GL
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearDepth(1.0);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDisable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glViewport(0, 0, demo.window_width, demo.window_height);
}

void main_loop() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // TODO: Draw the scene
  float aspect = (float)demo.window_width / (float)demo.window_height;
  demo.scene->draw(*demo.camera, aspect);

  SDL_GL_SwapWindow(demo.window);

  // Check for SDL events (user input, etc.)
  bool gui_input = false;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            demo.window_width = event.window.data1;
            demo.window_height = event.window.data2;
            glViewport(0, 0, demo.window_width, demo.window_height);
            break;
        }
        break;
      case SDL_QUIT:
        exit(EXIT_SUCCESS);
    }
  }
}

float sphere(float x, float y, float z) {
  static const float r = 0.6f;
  static const float a = 3.0f, b = 3.0f;
  return (x*x) / (a*a) + (y*y) / (b*b) + z*z - r*r;
}

float wave(float x, float y, float z) {
  return (cos(y) * sin(z)) / 5.0f;
}

float noise(float x, float y, float z) {
  return glm::simplex(glm::vec3(x, y, z) * 5.0f);
}

int main(int argc, char **argv) {
  // Initialize the graphics
  init_sdl();
  init_gl();

  // Create the graphics scene
  static const int repeat = 1; 
  demo.scene = new Scene;
  demo.camera = std::shared_ptr<OrthographicCamera>(
      new OrthographicCamera(
        0.0f, (float)repeat, 0.0f, (float)repeat,  // left, right, bottom, top
        1.0f, 1000.0f,  // near, far
        glm::vec3(0.0f, -5.0f, 0.0f),  // position
        glm::angleAxis(
          (float)M_PI / 2.0f,
          glm::vec3(1.0f, 0.0f, 0.0f))  // orientation
        ));
  demo.scene->addObject(demo.camera);

  // TODO: Set up our scene so that the image we render will tessellate nicely
  // in the background of the documentation. We do this by orienting the ortho
  // camera parallel to one of the sample grid axis, and then providing the
  // isosurface extraction algorithm a scalar field whose values wrap precicely
  // at the boundries of our view volume. Any triangles generated near one side
  // of the view volume boundry will be computed exactly the same as triangles
  // on the other side, because the scalar field values will be exactly the
  // same.
  std::shared_ptr<ScalarField> sf = std::shared_ptr<ScalarField>(
      new ScalarField(noise));
  demo.repeatingIsosurface = std::shared_ptr<RepeatingIsosurface>(
      new RepeatingIsosurface(sf,
        repeat, repeat,  // xRepeat, yRepeat
        glm::vec3(0.0f, 0.0f, 0.0f),  // position
        glm::angleAxis(
          (float)M_PI / 2.0f,
          glm::vec3(1.0f, 0.0f, 0.0f))  // orientation
        ));
  demo.repeatingIsosurface->setDrawOpaque(true);
  demo.repeatingIsosurface->setDrawWireframe(false);
  demo.scene->addObject(demo.repeatingIsosurface);

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(main_loop, 0, 1);
#else
  while (1) {
    main_loop();
    // TODO: Wait for VSync? Or should we poll input faster than that?
  }
#endif

  // TODO: Free SDL and GL resources
  delete demo.scene;

  return EXIT_SUCCESS;
}
