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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <png.h>

#include "debug.h"
#include "glError.h"
#include "scene.h"

#include "demo.h"

namespace mc { namespace samples {
  Demo::Demo(int argc, char **argv, const char *windowTitle)
    : m_windowTitle(windowTitle), m_scene(nullptr)
  {
    m_lastTime = 0.0f;
    // Parse the command line arguments
    m_argError = !m_parseArgs(argc, argv);
    if (m_argError)
      return;

    // Initialize the graphics
    m_initSdl();
    m_initGl();
    if (this->isScreenshot()) {
      m_initFramebuffer();
    }

    // Initialize the graphics scene
    m_initScene();
  }

  Demo::~Demo() {
    delete m_sceneString;

    // Free the graphics scene
    delete m_scene;
    if (this->isScreenshot()) {
      delete[] m_screenshot;

      if (!this->argError()) {
        // Free framebuffer resources
        glDeleteFramebuffers(1, &m_fbo);
        FORCE_ASSERT_GL_ERROR();
        glDeleteTextures(1, &m_fbColorBuffer);
        FORCE_ASSERT_GL_ERROR();
        glDeleteTextures(1, &m_fbDepthBuffer);
        FORCE_ASSERT_GL_ERROR();
      }
    }

    // TODO: Free GL resources
    // TODO: Free SDL resources
  }

  void Demo::m_printUsage() {
    // TODO
  }

  bool Demo::m_parseArgs(int argc, char **argv) {
    m_argError = false;
    m_screenshot = nullptr;
    m_sceneString = nullptr;
    m_width = 640;
    m_height = 480;
    m_algorithm = MC_DEFAULT_ALGORITHM;
    const char *optstring = "a:h:r:s:w:";
    const struct option longopts[] = {
      { "algorithm", required_argument, nullptr, 'a' },
      { "height", required_argument, nullptr, 'h' },
      { "scene", required_argument, nullptr, 's' },
      { "screenshot", required_argument, nullptr, 'r' },
      { "width", required_argument, nullptr, 'w' },
      { nullptr,     0,                 nullptr, 0 },
    };
    while (1) {
      int result;
      result = getopt_long(argc, argv, optstring, longopts, nullptr);
      if (result == -1)
        break;

      switch (result) {
        case 'a':
          // Set the isosurface extraction algorithm
          m_algorithm = mcAlgorithm_stringToFlag(optarg);
          if (m_algorithm == MC_UNKNOWN_ALGORITHM) {
            fprintf(stderr, "Error: Unknown algorithm '%s'\n", optarg);
            return false;
          }
          fprintf(stderr, "Using algorithm: '%s'\n", optarg);
          break;
        case 'h':
          // Set the window/screenshot height
          {
            char *end;
            m_height = strtol(optarg, &end, 10);
            if (end == optarg) {
              fprintf(stderr, "Error parsing height '%s'\n", optarg);
              return false;
            }
          }
          break;
        case 's':
          // String representing the scene
          {
            if (m_sceneString != nullptr) {
              fprintf(stderr,
                  "Error: multiple scenes specified\n");
              m_printUsage();
              return false;
            }
            size_t length = strnlen(optarg, 4096);
            m_sceneString = new char[length + 1];
            memcpy(m_sceneString, optarg, sizeof(char) * length);
            m_sceneString[length] = '\0';
          }
          break;
        case 'r':
          // Take a screenshot
          {
            if (m_screenshot != nullptr) {
              fprintf(stderr,
                  "Error: multiple screenshots specified\n");
              m_printUsage();
              return false;
            }
            size_t length = strnlen(optarg, 256);
            m_screenshot = new char[length + 1];
            memcpy(m_screenshot, optarg, sizeof(char) * length);
            m_screenshot[length] = '\0';
          }
          break;
        case 'w':
          // Set the window/screenshot width
          {
            char *end;
            m_width = strtol(optarg, &end, 10);
            if (end == optarg) {
              fprintf(stderr, "Error parsing width '%s'\n", optarg);
              return false;
            }
          }
          break;
        case '?':
          // Unknown option character
          m_printUsage();
          return false;
      }
    }

    return true;
  }

  void Demo::m_initSdl() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      fprintf(stderr, "Failed to initialize SDL: %s\n",
          SDL_GetError());
      exit(EXIT_FAILURE);
    }
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (m_screenshot)
      flags |= SDL_WINDOW_HIDDEN;
    m_window = SDL_CreateWindow(
        m_windowTitle,  // title
        SDL_WINDOWPOS_UNDEFINED,  // x
        SDL_WINDOWPOS_UNDEFINED,  // y
        m_width, m_height,  // w, h
        flags  // flags
        );
    if (m_window == nullptr) {
      fprintf(stderr, "Failed to create SDL window: %s\n",
          SDL_GetError());
      exit(EXIT_FAILURE);
    }
  }

  void Demo::m_initGl() {
    // Create an OpenGL context for our window
    m_glContext = SDL_GL_CreateContext(m_window);
    if (m_glContext == nullptr) {
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
    glViewport(0, 0, m_width, m_height);
  }

  void Demo::m_initFramebuffer() {
    // Create a GL framebuffer object for drawing to
    glGenFramebuffers(1, &m_fbo);
    FORCE_ASSERT_GL_ERROR();
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    FORCE_ASSERT_GL_ERROR();
    glGenTextures(1, &m_fbColorBuffer);
    FORCE_ASSERT_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, m_fbColorBuffer);
    FORCE_ASSERT_GL_ERROR();
    glTexImage2D(
        GL_TEXTURE_2D,  // target
        0,  // level
        GL_RGBA,  // internalFormat
        m_width,  // width
        m_height,  // height
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
        m_fbColorBuffer,  // texture
        0  // level
        );
    FORCE_ASSERT_GL_ERROR();
    glGenRenderbuffers(1, &m_fbDepthBuffer);
    FORCE_ASSERT_GL_ERROR();
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbDepthBuffer);
    FORCE_ASSERT_GL_ERROR();
    // NOTE: We could use more precision for the depth buffer on native GL, but
    // we are using 16 bits of precision for OpenGL ES support (e.g. WebGL).
    glRenderbufferStorage(
        GL_RENDERBUFFER,  // target
        GL_DEPTH_COMPONENT16,  // internalformat
        m_width,  // width
        m_height  // height
        );
    FORCE_ASSERT_GL_ERROR();
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,  // target
        GL_DEPTH_ATTACHMENT,  // attachment
        GL_RENDERBUFFER,  // renderbuffertarget
        m_fbDepthBuffer  // renderbuffer
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

  void Demo::m_initScene() {
    m_scene = new Scene();
    m_scene->addObject(Debug::instance());
  }

  void Demo::drawScreenshot() {
    assert(m_screenshot != nullptr);
    // Draw the scene to our texture framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    ASSERT_GL_ERROR();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float aspect = (float)m_width / (float)m_height;
    m_scene->draw(*m_camera, aspect);
    // Read the framebuffer to memory
    GLuint *pixels = new GLuint[m_width * m_height];
    glReadPixels(
        0,  // x
        0,  // y
        m_width,  // width
        m_height,  // height
        GL_RGBA,  // format
        GL_UNSIGNED_BYTE,  // type
        pixels  // data
        );
    ASSERT_GL_ERROR();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ASSERT_GL_ERROR();
    // Write the framebuffer to a PNG image
    FILE *fp;
    if (strcmp(m_screenshot, "-") == 0) {
      fp = stdout;
    } else {
      FILE *fp = fopen(m_screenshot, "wb");
      if (!fp) {
        fprintf(stderr, "Could not write screenshot to file '%s'\n",
            m_screenshot);
        exit(EXIT_FAILURE);
      }
    }
    png_structp png_ptr = png_create_write_struct(
        PNG_LIBPNG_VER_STRING,
        nullptr, nullptr, nullptr);
    if (!png_ptr) {
      fprintf(stderr, "Could not create PNG write struct\n");
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
        m_width,  // width
        m_height,  // height
        8,  // bit_depth
        PNG_COLOR_TYPE_RGB_ALPHA,  // color type
        PNG_INTERLACE_NONE,  // interlace type
        PNG_COMPRESSION_TYPE_DEFAULT,  // compression type
        PNG_FILTER_TYPE_DEFAULT  // filter method
        );
    png_write_info(png_ptr, info_ptr);
    for (int i = m_height - 1; i >= 0; --i) {
      png_write_row(png_ptr, (png_const_bytep)&pixels[i * m_width]);
    }
    png_write_end(png_ptr, nullptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    if (strcmp(m_screenshot, "-") != 0) {
      fclose(fp);
    }
    delete[] pixels;
  }

  void Demo::mainLoop() {
    if (m_lastTime == 0.0f) {
      // FIXME: The zero dt this causes might not be desirable.
      m_lastTime = (float)SDL_GetTicks() * 1000.0f;
    }

    // Check for SDL events (user input, etc.)
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (this->handleEvent(event))
        continue;  // Our derived class handled this event
      if (this->scene()->handleEvent(event))
        continue;  // One of the scene objects handled this event
      switch (event.type) {
        case SDL_WINDOWEVENT:
          switch (event.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
              m_width = event.window.data1;
              m_height = event.window.data2;
              glViewport(0, 0, m_width, m_height);
              break;
          }
          break;
        case SDL_QUIT:
          exit(EXIT_SUCCESS);
      }
    }

    // Calculate the time since the last frame was drawn
    float currentTime = (float)SDL_GetTicks() * 1000.0f;
    float dt = currentTime - m_lastTime;
    m_lastTime = currentTime;
    // Advance the scene simulation.
    m_scene->tick(dt);

    // Draw the window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_camera) {
      fprintf(stderr, "The scene camera was not set\n");
    } else {
      // Draw the scene
      float aspect = (float)m_width / (float)m_height;
      m_scene->draw(*m_camera, aspect);
    }

    SDL_GL_SwapWindow(m_window);
  }
} }
