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

#ifndef MC_ALGORITHMS_TRANSVOXEL_COMMON_H_
#define MC_ALGORITHMS_TRANSVOXEL_COMMON_H_

#define MC_TRANSVOXEL_NUM_REGULAR_CELLS 256
#define MC_TRANSVOXEL_NUM_CANONICAL_REGULAR_CELLS 18
#define MC_TRANSVOXEL_MAX_REGULAR_CELL_TRIANGLES 42  /* XXX: I don't know what this number actually is */

#define MC_TRANSVOXEL_NUM_TRANSITION_CELLS 512
#define MC_TRANSVOXEL_NUM_CANONICAL_TRANSITION_CELLS 73
#define MC_TRANSVOXEL_MAX_TRANSITION_CELL_EDGE_INTERSECTIONS 42  /* XXX: I don't know what this number actually is */
#define MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES 16
#define MC_TRANSVOXEL_NUM_TRANSITION_CELL_SAMPLES 9
#define MC_TRANSVOXEL_MAX_TRANSITION_CELL_TRIANGLES 42  /* XXX: I don't know what this number actually is */

typedef struct {
} mcTransvoxel_RegularCell;

typedef struct {
  int edges[MC_TRANSVOXEL_MAX_TRANSITION_CELL_EDGE_INTERSECTIONS];
} mcTransvoxel_TransitionCellEdgeIntersectionList;

typedef struct {
  int edgeIntersections[3];
} mcTransvoxel_Triangle;

typedef struct {
  mcTransvoxel_Triangle triangles[MC_TRANSVOXEL_MAX_REGULAR_CELL_TRIANGLES];
} mcTransvoxel_TransitionCellTriangleList;

typedef struct {
  mcTransvoxel_Triangle triangles[MC_TRANSVOXEL_MAX_TRANSITION_CELL_TRIANGLES];
} mcTransvoxel_RegularCellTriangleList;

#endif
