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
#include <png.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "../common/glError.h"
#include "../common/orthographicCamera.h"
#include "../common/scene.h"
#include "../common/template.h"
#include "repeatingIsosurface.h"

using namespace mc;
using namespace mc::samples;
using namespace mc::samples::screenshots;

struct demo {
  SDL_Window *window;
  SDL_GLContext glContext;
  int window_width, window_height;
  int fbWidth, fbHeight;
  GLuint fbo, fbColorBuffer, fbDepthBuffer;
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
  glClearColor(1.0, 0.0, 1.0, 0.0);
  glClearDepth(1.0);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDisable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glViewport(0, 0, demo.window_width, demo.window_height);
}

void init_framebuffer() {
  // TODO: Allow the user to specify the screenshot framebuffer size
  // TODO: Don't forget to call glViewport() for the framebuffer's size
  demo.fbWidth = 640;
  demo.fbHeight = 480;
  // TODO: Create a GL framebuffer object for drawing to
  glGenFramebuffers(1, &demo.fbo);
  FORCE_ASSERT_GL_ERROR();
  glBindFramebuffer(GL_FRAMEBUFFER, demo.fbo);
  FORCE_ASSERT_GL_ERROR();
  glGenTextures(1, &demo.fbColorBuffer);
  FORCE_ASSERT_GL_ERROR();
  glBindTexture(GL_TEXTURE_2D, demo.fbColorBuffer);
  FORCE_ASSERT_GL_ERROR();
  glTexImage2D(
      GL_TEXTURE_2D,  // target
      0,  // level
      GL_RGBA,  // internalFormat
      demo.fbWidth,  // width
      demo.fbHeight,  // height
      0,  // border
      GL_RGBA,  // format
      GL_UNSIGNED_INT_8_8_8_8,  // type
      nullptr  // data
      );
  FORCE_ASSERT_GL_ERROR();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  FORCE_ASSERT_GL_ERROR();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  FORCE_ASSERT_GL_ERROR();
  glFramebufferTexture2D(
      GL_FRAMEBUFFER,  // target
      GL_COLOR_ATTACHMENT0,  // attachment
      GL_TEXTURE_2D,  // textarget
      demo.fbColorBuffer,  // texture
      0  // level
      );
  FORCE_ASSERT_GL_ERROR();
  glGenRenderbuffers(1, &demo.fbDepthBuffer);
  FORCE_ASSERT_GL_ERROR();
  glBindRenderbuffer(GL_RENDERBUFFER, demo.fbDepthBuffer);
  FORCE_ASSERT_GL_ERROR();
  // NOTE: We could use more precision for the depth buffer on native GL, but
  // we are using 16 bits of precision for OpenGL ES support (e.g. WebGL).
  glRenderbufferStorage(
      GL_RENDERBUFFER,  // target
      GL_DEPTH_COMPONENT16,  // internalformat
      demo.fbWidth,  // width
      demo.fbHeight  // height
      );
  FORCE_ASSERT_GL_ERROR();
  glFramebufferRenderbuffer(
      GL_FRAMEBUFFER,  // target
      GL_DEPTH_ATTACHMENT,  // attachment
      GL_RENDERBUFFER,  // renderbuffertarget
      demo.fbDepthBuffer  // renderbuffer
      );
  FORCE_ASSERT_GL_ERROR();
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "GL framebuffer is not complete: 0x%04x\n",
        status);
    exit(EXIT_FAILURE);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  FORCE_ASSERT_GL_ERROR();
}

void destroy_framebuffer() {
  glDeleteFramebuffers(1, &demo.fbo);
  FORCE_ASSERT_GL_ERROR();
}

void main_loop() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // TODO: Draw the scene to the default framebuffer
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
  // Initialize the graphics
  init_sdl();
  init_gl();
  init_framebuffer();

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
      new ScalarField(terrain));
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

  // Draw the scene to our texture framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, demo.fbo);
  ASSERT_GL_ERROR();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  float aspect = (float)demo.fbWidth / (float)demo.fbHeight;
  demo.scene->draw(*demo.camera, aspect);
  // Read the framebuffer to memory
  GLuint *pixels = new GLuint[demo.fbWidth * demo.fbHeight];
  glReadPixels(
      0,  // x
      0,  // y
      demo.fbWidth,  // width
      demo.fbHeight,  // height
      GL_RGBA,  // format
      GL_UNSIGNED_BYTE,  // type
      pixels  // data
      );
  ASSERT_GL_ERROR();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  ASSERT_GL_ERROR();
  // Write the framebuffer to a PNG image
  const char *out = "out.png";
  FILE *fp = fopen(out, "wb");
  if (!fp) {
    fprintf(stderr, "Could not write to file '%s'\n", out);
    exit(EXIT_FAILURE);
  }
  png_structp png_ptr = png_create_write_struct(
      PNG_LIBPNG_VER_STRING,
      nullptr, nullptr, nullptr);
  if (!png_ptr) {
    fprintf(stderr, "Could not create PNG write struct\n", out);
    exit(EXIT_FAILURE);
  }
  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    exit(EXIT_FAILURE);
  }
  png_init_io(png_ptr, fp);
  png_set_IHDR(
      png_ptr, info_ptr,
      demo.fbWidth,  // width
      demo.fbHeight,  // height
      8,  // bit_depth
      PNG_COLOR_TYPE_RGB_ALPHA,  // color type
      PNG_INTERLACE_NONE,  // interlace type
      PNG_COMPRESSION_TYPE_DEFAULT,  // compression type
      PNG_FILTER_TYPE_DEFAULT  // filter method
      );
  png_write_info(png_ptr, info_ptr);
  for (int i = 0; i < demo.fbHeight; ++i) {
    png_write_row(png_ptr, (png_const_bytep)&pixels[i * demo.fbWidth]);
  }
  png_write_end(png_ptr, nullptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  delete[] pixels;

  // XXX: Test the Lua template engine
//  Template test("./test.txt");

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
