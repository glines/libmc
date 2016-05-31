#include <GL/glew.h>
#include <SDL.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <glm/gtc/quaternion.hpp>

#include "../common/arcballCamera.h"
#include "../common/scene.h"
#include "cubeObject.h"

using namespace mc::samples;
using namespace mc::samples::cubes;

struct demo {
  SDL_Window *window;
  SDL_GLContext glContext;
  int window_width, window_height;
  unsigned int cube;
  Scene *scene;
  std::shared_ptr<ArcballCamera> camera;
  std::shared_ptr<CubeObject> cubeObject;
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
      "256 Voxel Cubes",  // title
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
  glLineWidth(1.0);
  glViewport(0, 0, demo.window_width, demo.window_height);
}

void main_loop() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw the scene
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
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        demo.camera->mouseButtonEvent(event.button,
            demo.window_width, demo.window_height);
        break;
      case SDL_MOUSEMOTION:
        demo.camera->mouseMotionEvent(event.motion,
            demo.window_width, demo.window_height);
        break;
      case SDL_QUIT:
        exit(EXIT_SUCCESS);
    }
  }
}

int main(int argc, char **argv) {
  // Initialize the graphics
  init_sdl();
  init_gl();

  // Create the graphics scene
  demo.scene = new Scene;
  demo.camera = std::shared_ptr<ArcballCamera>(
      new ArcballCamera(
        90.0f,  // fovy
        0.1f,  // near
        1000.0f,  // far
        glm::vec3(0.0f, 0.0f, 2.0f),  // position
        glm::quat(),  // orientation
        glm::vec3(0.0f, 0.0f, 0.0f)  // followPoint
        ));
  demo.cubeObject = std::shared_ptr<CubeObject>(
      new CubeObject(0x05));
  demo.scene->addObject(demo.cubeObject);

  while (1) {
    main_loop();
    // TODO: Wait for VSync? Or should we poll input faster than that?
  }

  // TODO: Free SDL and GL resources
  delete demo.scene;

  return EXIT_SUCCESS;
}
