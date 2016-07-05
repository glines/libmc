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

void mcTransvoxel_computeCellRotationTable(
    unsigned int *table)
{
  /* Iterate over all transition cell configurations */
  for (unsigned int cell = 0; cell <= 0x1ff; ++cell) {
    /* Rotate this cell counterclockwise */
    unsigned int rotated = 0;
    rotated |= (cell & (1 << 0)) ? (1 << 6) : 0;
    rotated |= (cell & (1 << 1)) ? (1 << 3) : 0;
    rotated |= (cell & (1 << 2)) ? (1 << 0) : 0;
    rotated |= (cell & (1 << 3)) ? (1 << 7) : 0;
    rotated |= (cell & (1 << 4)) ? (1 << 4) : 0;
    rotated |= (cell & (1 << 5)) ? (1 << 1) : 0;
    rotated |= (cell & (1 << 6)) ? (1 << 8) : 0;
    rotated |= (cell & (1 << 7)) ? (1 << 5) : 0;
    rotated |= (cell & (1 << 8)) ? (1 << 2) : 0;
    table[cell] = rotated;
  }
}

void mcTransvoxel_computeCanonicalCellOrientationInversionTable(
    const unsigned int *rotationTable,
    unsigned int *table)
{
  /* Iterate over all transition cell configurations */
  for (unsigned int cell = 0; cell <= 0x1ff; ++cell) {
    /* Invert the transition cell */
    unsigned int inverted = cell;
    for (int i = 0; i < 2; ++i) {
      /* Rotate the transition cell four times */
      unsigned int rotated = inverted;
      for (int j = 0; j < 4; ++j) {
        rotated = rotationTable[rotated];
      }
      inverted = (~inverted) & 0x1ff;
    }
  }
}

int main(int argc, char **argv) {
  /* Allocate stack memory for each table */
  unsigned int cellRotationTable[512],
      canonicalCellRotationTable[512],
      canonicalCellOrientationInversionTable[512];

  /* Compute the tables */
  mcTransvoxel_computeCellRotationTable(
      cellRotationTable);
  mcTransvoxel_computeCanonicalCellOrientationInversionTable(
      cellRotationTable,
      canonicalCellOrientationInversionTable);
}
