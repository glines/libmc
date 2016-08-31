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

#include <mc/algorithms/coloredMarchingSquares/common.h>

int mcColoredMarchingSquares_sampleValue(int square, int sampleIndex) {
  assert(square >= 0);
  assert(square < MC_COLORED_MARCHING_SQUARES_NUM_SQUARES);
  assert(sampleIndex >= 0);
  assert(sampleIndex < 4);
  /* Return the nibble at the given index */
  return (square & (0x3 << (2 * sampleIndex))) >> (2 * sampleIndex);
}

#define SWAP_SQUARE_COLOR(a, b) \
  ((square & (0x3 << (2 * a))) >> (2 * a)) << (2 * b)
int mcColoredMarchingSquares_rotateSquare(int square) {
  int rotated = 0;
  rotated |= SWAP_SQUARE_COLOR(0, 1);
  rotated |= SWAP_SQUARE_COLOR(1, 3);
  rotated |= SWAP_SQUARE_COLOR(3, 2);
  rotated |= SWAP_SQUARE_COLOR(2, 0);
  return rotated;
}

int mcColoredMarchingSquares_mirrorSquare(int square) {
  int mirrored = 0;
  mirrored |= SWAP_SQUARE_COLOR(0, 1);
  mirrored |= SWAP_SQUARE_COLOR(1, 0);
  mirrored |= SWAP_SQUARE_COLOR(2, 3);
  mirrored |= SWAP_SQUARE_COLOR(3, 2);
  return mirrored;
}
