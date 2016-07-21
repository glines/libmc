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

#ifndef MC_SAMPLES_TRANSITION_INTERPOLATING_LATTICE_H_
#define MC_SAMPLES_TRANSITION_INTERPOLATING_LATTICE_H_

#include <mcxx/scalarField.h>
#include <mcxx/vector.h>
#include <vector>

namespace mc { namespace samples { namespace transition {
  class InterpolatingLattice : public mc::ScalarField {
    private:
      std::vector<std::vector<std::vector<float>>> m_samples;
      mc::Vec3 m_min, m_max;
      struct {
        int x, y, z;
      } m_latticeSize;
      float m_defaultSample;

      void m_growLattice(int x, int y, int z);
    public:
      InterpolatingLattice(
          const mc::Vec3 &min,
          const mc::Vec3 &max,
          float defaultSample = 0.0f);

      void setSample(int x, int y, int z, float value);
      float getSample(int x, int y, int z) const;

      float operator()(float x, float y, float z);
  };
} } }

#endif
