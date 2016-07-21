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

#include <assert.h>
#include <string.h>

#include "transvoxel_transform_tables.c"

#include <mc/algorithms/transvoxel/common.h>
#include <mc/algorithms/transvoxel/transform.h>

int mcTransvoxel_invertTransitionCell(int cell) {
  assert((cell & ~0x1ff) == 0);
  return ~cell & 0x1ff;
}

int mcTransvoxel_reflectTransitionCell(int cell) {
  assert((cell & ~0x1ff) == 0);
  return mcTransvoxel_transitionCellReflectionTable[cell];
}

int mcTransvoxel_rotateTransitionCell(int cell) {
  assert((cell & ~0x1ff) == 0);
  return mcTransvoxel_transitionCellRotationTable[cell];
}

int mcTransvoxel_rotateTransitionCellEdge(int edge) {
  assert(edge >= 0);
  assert(edge < MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES);
  return mcTransvoxel_transitionCellEdgeRotationTable[edge];
}

int mcTransvoxel_rotateTransitionCellEdgeReverse(int edge) {
  assert(edge >= 0);
  assert(edge < MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES);
  return mcTransvoxel_transitionCellEdgeReverseRotationTable[edge];
}

int mcTransvoxel_reflectTransitionCellEdge(int edge) {
  assert(edge >= 0);
  assert(edge < MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES);
  int table[] = {
    0,   // Edge 0
    3,   // Edge 1
    2,   // Edge 2
    1,   // Edge 3
    5,   // Edge 4
    4,   // Edge 5
    7,   // Edge 6
    6,   // Edge 7
    9,   // Edge 8
    8,   // Edge 9
    12,  // Edge 10
    11,  // Edge 11
    10,  // Edge 12
    15,  // Edge 13
    14,  // Edge 14
    13,  // Edge 15
  };
  return table[edge];
}

void mcTransvoxel_transitionCellSampleRelativePosition(
    int sampleIndex, int *pos)
{
  assert(sampleIndex >= 0);
  assert(sampleIndex < 9);
  static const int table[] = {
    0, 0,  /* Sample 0 */
    1, 0,  /* Sample 1 */
    2, 0,  /* Sample 2 */
    0, 1,  /* Sample 3 */
    1, 1,  /* Sample 4 */
    2, 1,  /* Sample 5 */
    0, 2,  /* Sample 6 */
    1, 2,  /* Sample 7 */
    2, 2,  /* Sample 8 */
  };
  memcpy(pos, &table[sampleIndex * 2], sizeof(int) * 2);
}
