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

namespace mc { namespace samples { namespace transition {
  /**
   * This scene object class demonstrates the transition between voxel meshes
   * of different resolutions. This is particularly useful when developing
   * algorithms such as Eric Lengyel's Transvoxel algorithm.
   *
   * Since voxel transitions involve much more complicated scenarios than with
   * ordinary marching cubes, the TransitionObject class cannot be configured
   * with a simple bitwise specification of the scalar field function as with
   * the CubeObject class. Instead, the TransitionObject class uses an octree
   * specification for representing the scalar field function that models the
   * different transition scenarios. Corners of each node in the octree are
   * given scalar field values, typically -1.0f or 1.0f.  These values are
   * interpolated within the volume of the octree in order to calculate the
   * scalar field function values.
   */
  class TransitionObject {
    public:
      TransitionObject(
          const glm::vec3 &position,
          const glm::quat &orientation);
  };
} } }
