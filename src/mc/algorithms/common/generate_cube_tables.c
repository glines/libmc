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

#include <stdio.h>
#include <stdlib.h>

enum Axis {
  X_AXIS, Y_AXIS, Z_AXIS
};

void computeRotationTable(int axis, unsigned int *table) {
  /* Iterate over all possible input cube configurations */
  for (unsigned int cube = 0; cube <= 0xff; ++cube) {
    unsigned int rotated = 0;
    /* Rotate the cube about the appropriate axis */
    switch (axis) {
      case X_AXIS:
        rotated |= (cube & (1 << 0)) ? (1 << 4) : 0;
        rotated |= (cube & (1 << 1)) ? (1 << 5) : 0;
        rotated |= (cube & (1 << 2)) ? (1 << 1) : 0;
        rotated |= (cube & (1 << 3)) ? (1 << 0) : 0;
        rotated |= (cube & (1 << 4)) ? (1 << 7) : 0;
        rotated |= (cube & (1 << 5)) ? (1 << 6) : 0;
        rotated |= (cube & (1 << 6)) ? (1 << 2) : 0;
        rotated |= (cube & (1 << 7)) ? (1 << 3) : 0;
        break;
      case Y_AXIS:
        rotated |= (cube & (1 << 0)) ? (1 << 3) : 0;
        rotated |= (cube & (1 << 1)) ? (1 << 0) : 0;
        rotated |= (cube & (1 << 2)) ? (1 << 1) : 0;
        rotated |= (cube & (1 << 3)) ? (1 << 2) : 0;
        rotated |= (cube & (1 << 4)) ? (1 << 7) : 0;
        rotated |= (cube & (1 << 5)) ? (1 << 4) : 0;
        rotated |= (cube & (1 << 6)) ? (1 << 5) : 0;
        rotated |= (cube & (1 << 7)) ? (1 << 6) : 0;
        break;
      case Z_AXIS:
        rotated |= (cube & (1 << 0)) ? (1 << 1) : 0;
        rotated |= (cube & (1 << 1)) ? (1 << 5) : 0;
        rotated |= (cube & (1 << 2)) ? (1 << 6) : 0;
        rotated |= (cube & (1 << 3)) ? (1 << 2) : 0;
        rotated |= (cube & (1 << 4)) ? (1 << 0) : 0;
        rotated |= (cube & (1 << 5)) ? (1 << 4) : 0;
        rotated |= (cube & (1 << 6)) ? (1 << 7) : 0;
        rotated |= (cube & (1 << 7)) ? (1 << 3) : 0;
        break;
    }
    /* Add the rotated cube to the table */
    table[cube] = rotated;
  }
}

void printRotationTable(int axis, const unsigned int *table) {
  const char *axisStr;
  switch (axis) {
    case X_AXIS:
      axisStr = "X";
      break;
    case Y_AXIS:
      axisStr = "Y";
      break;
    case Z_AXIS:
      axisStr = "Z";
      break;
  }
  fprintf(stdout, "static const unsigned int mcCubeRotationTable%s[] = {\n",
      axisStr);
  /* Iterate over all possible cube configurations and print the table */
  for (unsigned int cube = 0; cube <= 0xff; cube += 8) {
    fprintf(stdout, "  ");
    for (unsigned int i = 0; i < 8; ++i) {
      fprintf(stdout, "0x%02x,", table[cube + i]);
      if (i == 7)
        fprintf(stdout, "\n");
      else
        fprintf(stdout, " ");
    }
  }
  fprintf(stdout, "};\n");
}

void computeCanonicalOrientationTable(
    const unsigned int *x_table,
    const unsigned int *y_table,
    const unsigned int *z_table,
    unsigned int *table);

void printCanonicalOrientationTable(const unsigned int *table);

/**
 * This program generates the tables needed to quickly rotate a cube
 * configuration by right-angle increments with respect to the X, Y, or Z axis.
 * These tables are subsequently used to generate the canonical orientation
 * table, which can orient any cube in a canonical manner.
 */
int main(int argc, char **argv) {
  unsigned int *x_table, *y_table, *z_table, *canonical_table;
  /* Allocate memory for each table */
  x_table = malloc(sizeof(unsigned int) * 256);
  y_table = malloc(sizeof(unsigned int) * 256);
  z_table = malloc(sizeof(unsigned int) * 256);
  canonical_table = malloc(sizeof(unsigned int) * 256);

  /* Compute the X, Y, and Z rotation tables */
  computeRotationTable(X_AXIS, x_table);
  computeRotationTable(Y_AXIS, y_table);
  computeRotationTable(Z_AXIS, z_table);

  /* TODO: Compute the cannonical orientation table */
  /*
  computeCanonicalOrientationTable(
      x_table, y_table, z_table,
      canonical_table);
      */

  /* TODO: Print the tables */
  printRotationTable(X_AXIS, x_table);
  fprintf(stdout, "\n");
  printRotationTable(Y_AXIS, y_table);
  fprintf(stdout, "\n");
  printRotationTable(Z_AXIS, z_table);
  fprintf(stdout, "\n");
  /*
  printCanonicalOrientationTable(canonical_table);
  */

  /* Free table memory */
  free(canonical_table);
  free(z_table);
  free(y_table);
  free(x_table);

  return EXIT_SUCCESS;
}
