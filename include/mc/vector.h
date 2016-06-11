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

#ifndef MC_VECTOR_H_
#define MC_VECTOR_H_

typedef struct mcVec3 {
  float x, y, z;
} mcVec3;

/**
 * Computes the componentwise subtraction u - v and stores the result in w.
 */
void mcVec3_subtract(const mcVec3 *u, const mcVec3 *v, mcVec3 *w);

/**
 * Linearly interpolates between the two vectors using the given a as a weight
 * and returns the result.
 *
 * The result is computed as u * (1.0f - a) + v * a.
 */
mcVec3 mcVec3_lerp(const mcVec3 *u, const mcVec3 *v, float a);

/**
 * Computes the length of the given vector and returns the result.
 */
float mcVec3_length(const mcVec3 *u);

/**
 * Normalizes the vector u and stores the result in v.
 *
 * Note that u and v may point to the same vector.
 */
void mcVec3_normalize(const mcVec3 *u, mcVec3 *v);

/**
 * Computes the cross product for the given vectors u and v and stores the
 * resulting vector in w.
 */
void mcVec3_cross(const mcVec3 *u, const mcVec3 *v, mcVec3 *w);

/**
 * Computes the dot product of the given vectors and returns the result.
 */
float mcVec3_dot(const mcVec3 *u, const mcVec3 *v);

#endif
