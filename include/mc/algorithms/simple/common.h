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

#ifndef MC_ALGORITHMS_SIMPLE_COMMON_H_
#define MC_ALGORITHMS_SIMPLE_COMMON_H_

#define MC_SIMPLE_MAX_TRIANGLES 4

/**
 * A list of all edge intersections for a given cube configuration in marching
 * cubes.
 */
typedef struct mcSimpleEdgeIntersectionList {
  /** The edge intersections in this list. The maximum number of edge
   * intersections is 12, but if the actual number of edge intersections is
   * less than this then the tail of this array is filled with -1.
   *
   * \todo Define a macro for the maximum number of edge intersections in the
   * marching cubes algorithm, which happens to be 12.
   */
  int edges[12];
} mcSimpleEdgeIntersectionList;

/**
 * A triangle in the marching cubes algorithm defined by three edge
 * intersections.
 */
typedef struct mcSimpleTriangle {
  /** The three edge intersections that make up this triangle. The edge
   * intersections given in this array must also be listed in the corresponding
   * list of edge intersections for the given cube voxel configuration. */
  int edgeIntersections[3];
} mcSimpleTriangle;

/**
 * A list of triangles to be generated for a given voxel cube configuration in
 * the marching cubes algorithm. 
 */
typedef struct mcSimpleTriangleList {
  /** The list of triangles to generate for a given cube voxel configuration.
   * The maximum number of triangles that may be generated is
   * MC_SIMPLE_MAX_TRIANGLES, but if the actual number of triangles to generate
   * is less than this number, than the remaining triangles in the list will
   * have edge intersection indices of -1. */
  mcSimpleTriangle triangles[MC_SIMPLE_MAX_TRIANGLES];
} mcSimpleTriangleList;

#endif
