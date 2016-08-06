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

#include "../common/demo.h"
#include "../common/scene.h"
#include "../common/wasdCamera.h"
#include "transvoxelTerrain.h"

using namespace mc;
using namespace mc::samples;
using namespace mc::samples::transvoxel;

class TransvoxelDemo : public Demo {
  private:
    std::shared_ptr<WasdCamera> m_camera, m_stationaryCamera;
    std::shared_ptr<TransvoxelTerrain> m_terrain;
  public:
    TransvoxelDemo(int argc, char **argv)
      : Demo(argc, argv, "Transvoxel Demo")
    {
      if (this->argError())
        return;

      // Populate the graphics scene
      m_camera = std::shared_ptr<WasdCamera>(
          new WasdCamera(
            20.0f * ((float)M_PI / 180.0f),  // fovy
            0.1f,  // near
            1000.0f,  // far
            glm::vec3(0.0f, 0.0f, 15.0f),  // position
            glm::angleAxis(
              (float)M_PI / 4.0f,
              glm::vec3(1.0f, 0.0f, 0.0f))  // orientation
            ));
      m_stationaryCamera = std::shared_ptr<WasdCamera>(
          new WasdCamera(*m_camera));
      m_camera->setFar(10000.0);
      this->scene()->addObject(m_camera);
      this->setCamera(m_camera);
      m_terrain = std::shared_ptr<TransvoxelTerrain>(
          new TransvoxelTerrain(
            m_stationaryCamera,  // camera
            this->aspect(),  // aspect
            4  // minimumLod
            ));
      this->scene()->addObject(m_terrain);
    }
};

TransvoxelDemo *demo;

void main_loop() {
  demo->mainLoop();
}

int main(int argc, char **argv) {
  demo = new TransvoxelDemo(argc, argv);
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
