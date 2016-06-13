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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mc/algorithms/common/cube_definitions.h>

#define get_byte(num, byte) (((num) & (0xff << (8 * byte))) >> (8 * byte))
#define set_byte(num, byte, val) (((num) & ~(0xff << (8 * byte))) | ((val & 0xff) << (8 * byte)))
#define incr_byte(num, byte) (set_byte(num, byte, get_byte(num, byte) + 1))

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

void computeCanonicalOrientationInversions(
    const unsigned int *x_table,
    const unsigned int *y_table,
    const unsigned int *z_table,
    unsigned int *list,
    unsigned int *table,
    unsigned int *rotation_table)
{
  unsigned int listIndex;

  memset(table, -1, sizeof(unsigned int) * 256);
  memset(rotation_table, 0, sizeof(unsigned int) * 256);
  listIndex = 0;

  /* Rotations in the rotation table are encoded as follows. Starting from the
   * low-order byte, the first byte represents the number of rotations about
   * the z-axis, the second byte represents the number of rotations about the
   * x-axis, and the third byte represents the number of rotations about the
   * y-axis. Thus, the first three bytes represent 90-degree Euler angles in
   * zxy order.  The fourth byte is 0x01 if the cube was inverted, and 0x00
   * otherwis.
   *
   * If the inverse rotation is desired, simply apply the corresponding number
   * of reverse rotations in the yxz order.
   */

  /* Iterate over all possible cube configurations */
  for (unsigned int cube = 0; cube <= 0xff; ++cube) {
    int canonical = -1;
    /* Invert the cube */
    unsigned int inverted = cube;
    for (int i = 0; i < 2; ++i) {
      /* Iterate through all six possible directions the front face of the cube
       * can point */
      for (mcCubeFace face = 0; face < MC_CUBE_NUM_FACES; ++face) {
        unsigned int rotated, rotation;
        rotated = inverted;
        /* Set the inversion byte */
        rotation = set_byte(0, 3, i);
        /* Determine the face direction and rotate accordingly */
        switch (face) {
          case MC_CUBE_FACE_LEFT:
            rotated = z_table[rotated];
            rotation = incr_byte(rotation, 0);
          case MC_CUBE_FACE_BACK:
            rotated = z_table[rotated];
            rotation = incr_byte(rotation, 0);
          case MC_CUBE_FACE_RIGHT:
            rotated = z_table[rotated];
            rotation = incr_byte(rotation, 0);
          case MC_CUBE_FACE_FRONT:
            break;
          case MC_CUBE_FACE_BOTTOM:
            rotated = x_table[rotated];
            rotated = x_table[rotated];
            rotation = incr_byte(rotation, 1);
            rotation = incr_byte(rotation, 1);
          case MC_CUBE_FACE_TOP:
            rotated = x_table[rotated];
            rotation = incr_byte(rotation, 1);
            break;
        }
        /* Rotate about the Y axis four times */
        for (int j = 0; j < 4; ++j) {
          if (table[rotated] != -1) {
            if (canonical == -1) {
              /* We found the canonical orientation for this cube */
              canonical = table[rotated];
              table[cube] = canonical;
            } else {
              assert(table[rotated] == canonical);
            }
          }
          if (rotated == canonical) {
            /* We found the rotation sequence that brings us into the canonical
             * orientation for this cube */
            /* Store the rotation sequence and inversion flag */
            rotation_table[cube] = rotation;
          }
          rotated = y_table[rotated];
          rotation = incr_byte(rotation, 2);
        }
      }
      inverted = ~inverted & 0xff;
    }
    if (canonical == -1) {
      /* We could not find the current cube configuration in the table, so this
       * cube configuration defines a canonical cube orientation */
      canonical = cube;
      list[listIndex++] = canonical;
      table[cube] = canonical;
    }
  }
  assert(listIndex == 15);
}

void computeEdgeRotationTable(int axis, unsigned int *table) {
  /* This routine does not actually "compute" a table, since there are only 12
   * edges to consider. It is included for completeness, as we do actually
   * compute the reverse table.
   */
  static const unsigned int rotationTableX[] = {
     4, /* Edge 0 */
     9, /* Edge 1 */
     0, /* Edge 2 */
     8, /* Edge 3 */
     6, /* Edge 4 */
    11, /* Edge 5 */
     2, /* Edge 6 */
    10, /* Edge 7 */
     7, /* Edge 8 */
     5, /* Edge 9 */
     3, /* Edge 10 */
     1, /* Edge 11 */
  };
  static const unsigned int rotationTableY[] = {
     3, /* Edge 0 */
     0, /* Edge 1 */
     1, /* Edge 2 */
     2, /* Edge 3 */
     7, /* Edge 4 */
     4, /* Edge 5 */
     5, /* Edge 6 */
     6, /* Edge 7 */
    10, /* Edge 8 */
     8, /* Edge 9 */
    11, /* Edge 10 */
     9, /* Edge 11 */
  };
  static const unsigned int rotationTableZ[] = {
     9, /* Edge 0 */
     5, /* Edge 1 */
    11, /* Edge 2 */
     1, /* Edge 3 */
     8, /* Edge 4 */
     7, /* Edge 5 */
    10, /* Edge 6 */
     3, /* Edge 7 */
     0, /* Edge 8 */
     4, /* Edge 9 */
     2, /* Edge 10 */
     6, /* Edge 11 */
  };
  switch (axis) {
    case X_AXIS:
      memcpy(table, rotationTableX, sizeof(rotationTableX));
    break;
    case Y_AXIS:
      memcpy(table, rotationTableY, sizeof(rotationTableY));
    break;
    case Z_AXIS:
      memcpy(table, rotationTableZ, sizeof(rotationTableZ));
    break;
  }
}

void computeReverseEdgeRotationTable(
    const unsigned int *table,
    unsigned int *reverseTable)
{
  /* Iterate over all cube edges */
  for (int i = 0; i < MC_CUBE_NUM_EDGES; ++i) {
    /* Rotate the edge by 90-degrees three times, which is equivalent to
     * rotating the edge in the opposite direction by 90-degrees. */
    reverseTable[i] = table[i];
    reverseTable[i] = table[reverseTable[i]];
    reverseTable[i] = table[reverseTable[i]];
  }
}

void printCubeCharTable(const unsigned int *table, FILE *fp) {
  /* Iterate over all possible cube configurations and print the table */
  for (unsigned int cube = 0; cube <= 0xff; cube += 8) {
    fprintf(fp, "  ");
    for (unsigned int i = 0; i < 8; ++i) {
      fprintf(fp, "0x%02x,", table[cube + i]);
      if (i == 7)
        fprintf(fp, "\n");
      else
        fprintf(fp, " ");
    }
  }
}

void printCubeIntTable(const unsigned int *table, FILE *fp) {
  /* Same routine as printCubeCharTable(), except for printing integers */
  /* Iterate over all possible cube configurations and print the table */
  for (unsigned int cube = 0; cube <= 0xff; cube += 4) {
    fprintf(fp, "  ");
    for (unsigned int i = 0; i < 4; ++i) {
      fprintf(fp, "0x%08x,", table[cube + i]);
      if (i == 3)
        fprintf(fp, "\n");
      else
        fprintf(fp, " ");
    }
  }
}

void printRotationTable(int axis, const unsigned int *table, FILE *fp) {
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
  fprintf(fp, "const unsigned int mcCube_rotationTable%s[] = {\n",
      axisStr);
  printCubeCharTable(table, fp);
  fprintf(fp, "};\n");
}

void printCanonicalOrientationInversionList(const unsigned int *list, FILE *fp) {
  fprintf(fp, "typedef enum mcCubeCanonicalOrientationInversion {\n");
  for (int i = 0; i < 15; ++i) {
    fprintf(fp, "  MC_CUBE_CANONICAL_ORIENTATION_INVERSION_%d = 0x%02x,\n",
        i, list[i]);
  }
  fprintf(fp, "} mcCubeCanonicalOrientationInversion;\n");
}

void printCanonicalOrientationInversionTable(const unsigned int *table, FILE *fp) {
  fprintf(fp,
      "const unsigned int mcCube_canonicalOrientationInversionTable[] = {\n");
  printCubeCharTable(table, fp);
  fprintf(fp, "};\n");
}

void printCanonicalRotationInversionTable(const unsigned int *table, FILE *fp) {
  fprintf(fp,
      "const unsigned int mcCube_canonicalRotationInversionSequenceTable[] = {\n");
  printCubeIntTable(table, fp);
  fprintf(fp, "};\n");
}

void printEdgeRotationTable(
    int axis, int reverse, const unsigned int *table, FILE *fp)
{
  const char *axisStr, *reverseStr;
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
  if (reverse)
    reverseStr = "Reverse";
  else 
    reverseStr = "";
  fprintf(fp,
      "const unsigned int mcCube_edge%sRotationTable%s[] = {\n",
      reverseStr, axisStr);
  for (int i = 0; i < MC_CUBE_NUM_EDGES; ++i) {
    fprintf(fp, "  %2d, /* Edge %d */\n", table[i], i);
  }
  fprintf(fp, "};\n");
}

void print_usage() {
  fprintf(stderr,
      "Usage:\n"
      "generate_cube_tables [cube_tables.c|canonical_cube_orientations.h]\n");
}

typedef enum TableFile {
  CUBE_TABLES_C,
  CANONICAL_CUBE_ORIENTATIONS_H,
} TableFile;

/**
 * This program generates the tables needed to quickly rotate a cube
 * configuration by right-angle increments with respect to the X, Y, or Z axis.
 * These tables are subsequently used to generate the canonical orientation
 * table, which can orient any cube in a canonical manner.
 */
int main(int argc, char **argv) {
  TableFile tableFile;
  /* Allocate stack memory for each table */
  unsigned int x_table[256], y_table[256], z_table[256],
               canonical_orientation_inversion_list[15],
               canonical_orientation_inversion_table[256],
               canonical_rotation_inversion_table[256],
               x_edge_table[MC_CUBE_NUM_EDGES],
               y_edge_table[MC_CUBE_NUM_EDGES],
               z_edge_table[MC_CUBE_NUM_EDGES],
               x_reverse_edge_table[MC_CUBE_NUM_EDGES],
               y_reverse_edge_table[MC_CUBE_NUM_EDGES],
               z_reverse_edge_table[MC_CUBE_NUM_EDGES];

  /* Parse command line arguments to determine which table we are generating */
  if (argc != 2) {
    print_usage();
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "cube_tables.c") == 0) {
    tableFile = CUBE_TABLES_C;
  } else if (strcmp(argv[1], "canonical_cube_orientations.h") == 0) {
    tableFile = CANONICAL_CUBE_ORIENTATIONS_H;
  } else {
    print_usage();
    return EXIT_FAILURE;
  }

  /* Compute the X, Y, and Z rotation tables */
  computeRotationTable(X_AXIS, x_table);
  computeRotationTable(Y_AXIS, y_table);
  computeRotationTable(Z_AXIS, z_table);

  /* Compute the cannonical orientation inversion tables */
  computeCanonicalOrientationInversions(
      x_table, y_table, z_table,
      canonical_orientation_inversion_list,
      canonical_orientation_inversion_table,
      canonical_rotation_inversion_table);

  /* TODO: Compute the cannonical orientation tables */

  /* Compute the X, Y, and Z edge rotation tables */
  computeEdgeRotationTable(X_AXIS, x_edge_table);
  computeEdgeRotationTable(Y_AXIS, y_edge_table);
  computeEdgeRotationTable(Z_AXIS, z_edge_table);

  /* Compute the reverse X, Y, and Z edge rotation tables */
  computeReverseEdgeRotationTable(x_edge_table, x_reverse_edge_table);
  computeReverseEdgeRotationTable(y_edge_table, y_reverse_edge_table);
  computeReverseEdgeRotationTable(z_edge_table, z_reverse_edge_table);

  /* Print the tables */
  /* NOTE: stdout is used because Emscripten's filesystem model makes using
   * fopen() difficult */
  switch (tableFile) {
    case CUBE_TABLES_C:
      {
        /* Print the tables */
        printRotationTable(X_AXIS, x_table, stdout);
        fprintf(stdout, "\n");
        printRotationTable(Y_AXIS, y_table, stdout);
        fprintf(stdout, "\n");
        printRotationTable(Z_AXIS, z_table, stdout);
        fprintf(stdout, "\n");
        printCanonicalOrientationInversionTable(
            canonical_orientation_inversion_table, stdout);
        fprintf(stdout, "\n");
        printCanonicalRotationInversionTable(
            canonical_rotation_inversion_table, stdout);
        fprintf(stdout, "\n");
        printEdgeRotationTable(X_AXIS, 0, x_edge_table, stdout);
        fprintf(stdout, "\n");
        printEdgeRotationTable(Y_AXIS, 0, y_edge_table, stdout);
        fprintf(stdout, "\n");
        printEdgeRotationTable(Z_AXIS, 0, z_edge_table, stdout);
        fprintf(stdout, "\n");
        printEdgeRotationTable(X_AXIS, 1, x_reverse_edge_table, stdout);
        fprintf(stdout, "\n");
        printEdgeRotationTable(Y_AXIS, 1, y_reverse_edge_table, stdout);
        fprintf(stdout, "\n");
        printEdgeRotationTable(Z_AXIS, 1, z_reverse_edge_table, stdout);
      }
      break;
    case CANONICAL_CUBE_ORIENTATIONS_H:
      {
        /* We need to put the enumeration of canonical cube orientations in a
         * separate file because it is not possible to link enum types into object
         * files. */
        /* Print the include guard */
        fprintf(stdout,
            "#ifndef MC_ALGORITHMS_COMMON_CANONICAL_CUBE_ORIENTATIONS_H_\n"
            "#define MC_ALGORITHMS_COMMON_CANONICAL_CUBE_ORIENTATIONS_H_\n\n");
        printCanonicalOrientationInversionList(
            canonical_orientation_inversion_list, stdout);
        /* Print the include guard */
        fprintf(stdout, "\n#endif\n");
      }
      break;
  }

  return EXIT_SUCCESS;
}
