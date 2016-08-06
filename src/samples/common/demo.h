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
#include <memory>

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
      std::shared_ptr<Camera> m_camera;
      bool m_argError;
      char *m_sceneString, *m_screenshot;
      mcAlgorithmFlag m_algorithm;
      float m_lastTime;

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
       * \return The current width of the demo window in pixels.
       */
      int width() const { return m_width; }

      /**
       * \return The current height of the demo window in pixels.
       */
      int height() const { return m_height; }

      /**
       * Computes and returns the aspect ratio of the demo window, which is
       * window width over window height.
       * \return The current aspect ratio of the demo window.
       */
      float aspect() const { return (float)m_width / (float)m_height; }

      /**
       * \return Pointer to the graphics scene for this demo.
       */
      Scene *scene() { return m_scene; }

      /**
       * Configures the graphics scene according to some string whose format is
       * defined by derived classes. This method can be implemented by derived
       * classes to allow the user to pass strings through the --scene flag to
       * affect the configuration of the scene being drawn. Derived classes can
       * specify any format for the string representing the scene.
       *
       * \param scene String representing the graphics scene to set up.
       * \return True if the given scene string was valid, false otherwise.
       */
      virtual bool setSceneString(const char *scene) { return false; }
      /**
       * Returns the string representing the inital configuration of the
       * graphics scene. This string's encoding is defined by derived classes.
       *
       * \return String representing the initial configuration of the graphics
       * scene.
       */
      const char *sceneString() const { return m_sceneString; }

      /**
       * Sets the camera to be used when drawing the graphics scene.
       *
       * \param camera Shared pointer to the camera to use when drawing the
       * scene.
       */
      void setCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }

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
       * \sa isScreenshot()
       */
      void drawScreenshot();

      /**
       * Derived classes can implement this method to handle arbitrary SDL
       * events. If the given event is handled, this method should return true.
       * By returning true, derived classes can mask the default event handling
       * behavior.
       *
       * \param event The SDL event structure for the event to handle.
       * \return True if the event was handled, and false otherwise.
       */
      virtual bool handleEvent(const SDL_Event &event) {
        return false;
      }

      /**
       * Executes the body of the main loop of the demo. This should be called
       * every frame. Drawing an input is handled in the same loop.
       */
      void mainLoop();
  };
} }

/** @} */

/** @} */

#endif
