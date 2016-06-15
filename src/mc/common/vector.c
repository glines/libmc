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

void mcVec3_add(const mcVec3 *u, const mcVec3 *v, mcVec3 *w) {
  w->x = u->x + v->x;
  w->y = u->y + v->y;
  w->z = u->z + v->z;
}

void mcVec3_subtract(const mcVec3 *u, const mcVec3 *v, mcVec3 *w) {
  w->x = u->x - v->x;
  w->y = u->y - v->y;
  w->z = u->z - v->z;
}

void mcVec3_scalarProduct(float a, const mcVec3 *u, mcVec3 *v) {
  v->x = a * u->x;
  v->y = a * u->y;
  v->z = a * u->z;
}

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

void mcVec3_normalize(const mcVec3 *u, mcVec3 *v) {
  float length = mcVec3_length(u);
  v->x = u->x / length;
  v->y = u->y / length;
  v->z = u->z / length;
}

void mcVec3_cross(const mcVec3 *u, const mcVec3 *v, mcVec3 *w) {
  w->x = u->y * v->z - u->z * v->y;
  w->y = u->z * v->x - u->x * v->z;
  w->z = u->x * v->y - u->y * v->x;
}

/**
 * Computes the dot product of the given vectors and returns the result.
 */
float mcVec3_dot(const mcVec3 *u, const mcVec3 *v) {
  return u->x * v->x + u->y * v->y + u->z * v->z;
}
