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

#ifndef MC_SAMPLES_COMMON_ISOSURFACE_OBJECT_H_
#define MC_SAMPLES_COMMON_ISOSURFACE_OBJECT_H_

#include <GL/glew.h>
#include <mcxx/scalarField.h>
#include <mcxx/vector.h>

extern "C" {
#include <mc/algorithms.h>
}

#include "meshObject.h"

namespace mc { namespace samples {
  class IsosurfaceObject : public MeshObject {
    private:
      std::shared_ptr<ScalarField> m_sf;
      Vec3 m_min, m_max;
      mcAlgorithmFlag m_algorithm;
      int m_res;

      void m_update();
    public:
      IsosurfaceObject(
          const glm::vec3 &position = glm::vec3(0.0f),
          const glm::quat &orientation = glm::quat());

      void setAlgorithm(mcAlgorithmFlag algorithm);
      void setScalarField(std::shared_ptr<ScalarField> sf);
      void setMin(const Vec3 &min);
      void setMax(const Vec3 &max);
      void setres(int res);
  };
} }

#endif
