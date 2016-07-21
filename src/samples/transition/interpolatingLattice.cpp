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

#include <cassert>
#include <cmath>

#include "interpolatingLattice.h"

#define max(x, y) ((x) < (y) ? (y) : (x))

namespace mc { namespace samples { namespace transition {
  InterpolatingLattice::InterpolatingLattice(
      const mc::Vec3 &min,
      const mc::Vec3 &max,
      float defaultSample)
    : m_min(min), m_max(max), m_defaultSample(defaultSample)
  {
    m_latticeSize.x = 0;
    m_latticeSize.y = 0;
    m_latticeSize.z = 0;
  }

  void InterpolatingLattice::m_growLattice(int x, int y, int z) {
    if (x >= m_samples.size()) {
      m_samples.resize(x + 1);
      m_latticeSize.x = max(m_latticeSize.x, x);
    }
    if (y >= m_samples[x].size()) {
      m_samples[x].resize(y + 1);
      m_latticeSize.y = max(m_latticeSize.y, y);
    }
    if (z >= m_samples[x][y].size()) {
      m_samples[x][y].resize(z + 1, m_defaultSample);
      m_latticeSize.z = max(m_latticeSize.z, z);
    }
  }

  void InterpolatingLattice::setSample(int x, int y, int z, float value) {
    assert(x >= 0);
    assert(y >= 0);
    assert(z >= 0);
    m_growLattice(x, y, z);
    m_samples[x][y][z] = value;
  }

  float InterpolatingLattice::getSample(int x, int y, int z) const {
    if (x < 0 || y < 0 || z < 0)
      return m_defaultSample;
    if (x >= m_samples.size())
      return m_defaultSample;
    if (y >= m_samples[x].size())
      return m_defaultSample;
    if (z >= m_samples[x][y].size())
      return m_defaultSample;
    return m_samples[x][y][z];
  }

  float InterpolatingLattice::operator()(float x, float y, float z) {
    // Convert the given coordinates to lattice coordinates
    struct {
      float x, y, z;
    } latticePos;
#define LATTICE_POS(axis) \
    latticePos.axis = (((axis - m_min.axis())  / ((m_max.axis() - m_min.axis()))) * (float)m_latticeSize.axis)
    LATTICE_POS(x);
    LATTICE_POS(y);
    LATTICE_POS(z);
    // Compute the lattice position for the given coordinates
    struct {
      int x, y, z;
    } latticeIndex;
#define LATTICE_INDEX(axis) \
    latticeIndex.axis = (int)floor(latticePos.axis);
    LATTICE_INDEX(x);
    LATTICE_INDEX(y);
    LATTICE_INDEX(z);
    // Normalize the given coordinates to the lattice cube of interest
    struct {
      float x, y, z;
    } normPos;
#define NORM_POS(axis) \
    normPos.axis = latticePos.axis - (float)latticeIndex.axis;
    NORM_POS(x);
    NORM_POS(y);
    NORM_POS(z);
    // Perform trilinear interpolation on the nearest sample points
    float result = 0.0f;
    for (int zIndex = 0; zIndex < 2; ++zIndex) {
      for (int yIndex = 0; yIndex < 2; ++yIndex) {
        for (int xIndex = 0; xIndex < 2; ++xIndex) {
          float value = this->getSample(
              latticePos.x + xIndex,
              latticePos.y + yIndex,
              latticePos.z + zIndex);
          result +=
            ((xIndex ? normPos.x : 1.0f - normPos.x)
            * (yIndex ? normPos.y : 1.0f - normPos.y)
            * (zIndex ? normPos.z : 1.0f - normPos.z)
            ) * value;
        }
      }
    }
    return result;
  }
} } }
