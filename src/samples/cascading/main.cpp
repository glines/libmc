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

extern "C" {
#include <mc/common/quadNode.h>
}

#include "../common/demo.h"
#include "../common/orthographicCamera.h"
#include "../common/scene.h"
#include "../common/wasdCamera.h"
#include "tree.h"

namespace mc { namespace samples { namespace cascading {
  /** This routine generates a libmc quadtree that approximates a circle */
  void generateQuadtree() {
    mcQuadNode root;
    mcQuadNode_initRoot(&root);
    mcQuadNodeCoordinates pos;
    int level = 4;
    while (level >= 0) {
      for (int y = 0; y < 30; ++y) {
        for (int x = 0; x < 30; ++x) {
          pos.coord[0] = x - 15;
          pos.coord[1] = y - 15;
          mcQuadNode *node = mcQuadNode_getNode(&root, &pos, level);
        }
      }
      level--;
    }
    mcQuadNode_destroy(&root);
  }

  class CascadingDemo : public Demo {
    private:
      std::shared_ptr<OrthographicCamera> m_camera;
      std::shared_ptr<WasdCamera> m_wasdCamera;
      std::shared_ptr<Tree> m_tree;
    public:
      CascadingDemo(int argc, char **argv)
        : Demo(argc, argv, "Cascading Transition Cell Demo")
      {
        if (this->argError())
          return;

        // Populate the graphics scene
        m_camera = std::shared_ptr<OrthographicCamera>(
            new OrthographicCamera(
              -10.0f,  // left
              10.0f,  // right
              -10.0f,  // top
              10.0f,  // bottom
              0.1f,  // near
              1000.0f,  // far
              glm::vec3(0.0f, 0.0f, 15.0f)  // position
              ));
        this->scene()->addObject(m_camera);
        m_wasdCamera = std::shared_ptr<WasdCamera>(
            new WasdCamera(
              80.0f * ((float)M_PI / 180.0f),  // fovy
              0.1f,  // near
              100000.0f,  // far
              glm::vec3(0.0f, 0.0f, 15.0f),  // position
              glm::quat()  // orientation
              ));
//        this->scene()->addObject(m_wasdCamera);
        this->setCamera(m_camera);
        m_tree = std::shared_ptr<Tree>(
            new Tree(m_camera));
        this->scene()->addObject(m_tree);
      }
  };
} } }

using namespace mc::samples::cascading;

CascadingDemo *demo;

void main_loop() {
  demo->mainLoop();
}

int main(int argc, char **argv) {
  generateQuadtree();  /* XXX */
  demo = new CascadingDemo(argc, argv);
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
