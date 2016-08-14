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

#ifndef MC_ALGORITHMS_MARCHING_SQUARES_COMMON_H_
#define MC_ALGORITHMS_MARCHING_SQUARES_COMMON_H_

#include <mc/contour.h>

#define MC_MARCHING_SQUARES_NUM_CANONICAL_SQUARES 4
#define MC_MARCHING_SQUARES_MAX_NUM_LINES 2

typedef struct {
  int edges[4];
} mcMarchingSquares_EdgeIntersectionList;

typedef struct {
  /* FIXME: mcLine defined in mc/contour.h is intended to be used with vertex
   * indices, while mcLine here is used with edge intersections... */
  mcLine lines[2];
} mcMarchingSquares_LineList;

#endif
