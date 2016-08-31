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

#include <mc/algorithms/common/square.h>

void mcSquare_sampleRelativePosition(int sampleIndex, int *pos) {
  pos[0] = (sampleIndex & (1 << 0)) >> 0;
  pos[1] = (sampleIndex & (1 << 1)) >> 1;
}

void mcSquare_edgeSampleIndices(int edgeIndex, int *sampleIndices) {
  typedef struct {
    int indices[2];
  } SampleIndexPair;
  static const SampleIndexPair table[] = {
    { .indices = { 0, 1 } },  /* Edge 0 */
    { .indices = { 1, 3 } },  /* Edge 1 */
    { .indices = { 3, 2 } },  /* Edge 2 */
    { .indices = { 2, 0 } },  /* Edge 3 */
  };
  assert(edgeIndex >= 0);
  assert(edgeIndex < 4);
  sampleIndices[0] = table[edgeIndex].indices[0];
  sampleIndices[1] = table[edgeIndex].indices[1];
}

int mcSquare_sampleValue(int square, int sampleIndex) {
  assert(sampleIndex >= 0);
  assert(sampleIndex < 4);
  return (square & (1 << sampleIndex)) >> sampleIndex;
}

int mcSquare_rotateSquare(int square) {
  assert(square >= 0);
  assert(square <= 0xf);
  int rotated = 0;
  rotated |= square & (1 << 0) ? (1 << 1) : 0;
  rotated |= square & (1 << 1) ? (1 << 3) : 0;
  rotated |= square & (1 << 2) ? (1 << 0) : 0;
  rotated |= square & (1 << 3) ? (1 << 2) : 0;
  assert(rotated >= 0);
  assert(rotated <= 0xf);
  return rotated;
}

int mcSquare_invertSquare(int square) {
  assert(square >= 0);
  assert(square <= 0xf);
  int inverted = ~square & 0xf;
  assert(inverted >= 0);
  assert(inverted <= 0xf);
  return inverted;
}

int mcSquare_rotateEdge(int edge) {
  assert(edge >= 0);
  assert(edge < 4);
  static const int table[] = {
    1,  /* Edge 0 */
    2,  /* Edge 1 */
    3,  /* Edge 2 */
    0,  /* Edge 3 */
  };
  return table[edge];
}

int mcSquare_rotateEdgeReverse(int edge) {
  assert(edge >= 0);
  assert(edge < 4);
  static const int table[] = {
    3,  /* Edge 0 */
    0,  /* Edge 1 */
    1,  /* Edge 2 */
    2,  /* Edge 3 */
  };
  return table[edge];
}

int mcSquare_mirrorEdge(int edge) {
  assert(edge >= 0);
  assert(edge < 4);
  static const int table[] = {
    0,  /* Edge 0 */
    3,  /* Edge 1 */
    2,  /* Edge 2 */
    1,  /* Edge 3 */
  };
  return table[edge];
}
