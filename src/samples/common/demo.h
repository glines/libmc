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

#ifndef MC_SAMPLES_COMMON_DEMO_H_
#define MC_SAMPLES_COMMON_DEMO_H_

/**
 * \addtogroup samples
 * @{
 */

/**
 * \addtogroup common
 * @{
 */

#include <GL/glew.h>
#include <SDL.h>

extern "C" {
#include <mc/algorithms.h>
}

namespace mc { namespace samples {
  class Camera;
  class Scene;
  /**
   * This class encapsulates the boilerplate code used in any graphical demo.
   * Demo programs can subclass this class to quickly and easily set up an
   * OpenGL window for drawing, along with other features such as parsing
   * common command line arguments and the ability to take screenshots.
   */
  class Demo {
    private:
      const char *m_windowTitle;
      SDL_Window *m_window;
      SDL_GLContext m_glContext;
      GLuint m_fbo, m_fbColorBuffer, m_fbDepthBuffer;
      int m_width, m_height;
      Scene *m_scene;
      bool m_argError;
      char *m_screenshot;
      mcAlgorithmFlag m_algorithm;

      void m_printUsage();
      bool m_parseArgs(int argc, char **argv);

      void m_initSdl();
      void m_initGl();
      void m_initFramebuffer();
      void m_initScene();
    public:
      /**
       * Constructs an object for the current demo program with the given
       * command line arguments. These command line arguments should be passed
       * directly from main(). The arguments are parsed for common demo
       * parameters such as window dimensions.
       *
       * \param argc The number of arguments in argv. This includes the first
       * argument, which is the name of the image of the command invoked.
       * \param argv Array of the command line arguments passed.
       * \param windowTitle String to use for the SDL window's title bar.
       *
       * Errors during command line argument parsing are indicated with the
       * argError() flag set to true.
       */
      Demo(int argc, char **argv, const char *windowTitle);
      /**
       * Destroys this demo object and frees all resources associated with it.
       */
      ~Demo();

      /**
       * \return True if there was an error parsing the command line arguments.
       * False otherwise.
       */
      bool argError() const { return m_argError; }

      /**
       * \return Flag representing the isosurface extractiov algorithm selected
       * through the command line arguments.
       */
      mcAlgorithmFlag algorithm() const { return m_algorithm; }

      /**
       * \return Pointer to the graphics scene for this demo.
       */
      Scene *scene() { return m_scene; }

      /**
       * \return True if a screenshot is being taken, false otherwise.
       *
       * \sa drawScreenshot()
       */
      bool isScreenshot() const {
        return m_screenshot == nullptr ? false : true;
      }

      /**
       * Draw a screenshot of the current scene and write the screenshot to
       * file. The file to write is given by the command line arguments. This
       * method must only be called if isScreenshot() returns true.
       *
       * \param camera The camera to use when drawing the scene.
       *
       * \sa isScreenshot()
       */
      void drawScreenshot(const Camera &camera);

      /**
       * Executes the body of the main loop of the demo. This should be called
       * every frame. Drawing an input is handled in the same loop.
       *
       * \param camera The camera with which to draw the scene.
       */
      void mainLoop(const Camera &camera);
  };
} }

/** @} */

/** @} */

#endif
