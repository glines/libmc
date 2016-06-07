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

#include <math.h>

#include <mc/vector.h>

mcVec3 mcVec3_lerp(const mcVec3 *u, const mcVec3 *v, float a) {
  mcVec3 result;
  result.x = (1.0f - a) * u->x + a * v->x;
  result.y = (1.0f - a) * u->y + a * v->y;
  result.z = (1.0f - a) * u->z + a * v->z;
  return result;
}

float mcVec3_length(const mcVec3 *u) {
  return sqrt(u->x * u->x + u->y * u->y + u->z * u->z);
}

void mcVec3_normalize(mcVec3 *u) {
  float length = mcVec3_length(u);
  u->x /= length;
  u->y /= length;
  u->z /= length;
}
