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

#include <mc/algorithms/common/cube.h>
#include <mc/algorithms/transvoxel/common.h>
#include <mc/algorithms/transvoxel/transform.h>

#define get_byte(num, byte) (((num) & (0xff << (8 * byte))) >> (8 * byte))
#define set_byte(num, byte, val) (((num) & ~(0xff << (8 * byte))) | ((val & 0xff) << (8 * byte)))
#define incr_byte(num, byte) (set_byte(num, byte, get_byte(num, byte) + 1))

void mcTransvoxel_computeCanonicalRegularCellTable(
    int *list,
    int *table,
    int *sequenceTable)
{
  unsigned int listIndex;

  memset(table, -1, sizeof(int) * 256);
  memset(sequenceTable, -1, sizeof(int) * 256);
  listIndex = 0;

  /* Iterate over all possible cell configurations */
  for (int cell = 0; cell <= 0xff; ++cell) {
    int canonical = -1;
    /* Iterate to invert the cube */
    int inverted = cell;
    for (int i = 0; i < 2; ++i) {
      /* Rotate the cell in all directions */
      for (mcCubeFace face = 0; face < MC_CUBE_NUM_FACES; ++face) {
        int rotated, sequence;
        /* Set the inversion byte */
        sequence = set_byte(0, 3, i);
        rotated = inverted;
        /* Determine the face direction and rotate accordingly */
        switch (face) {
          case MC_CUBE_FACE_LEFT:
            rotated = mcCube_rotateCubeZ(rotated);
            sequence = incr_byte(sequence, 0);
          case MC_CUBE_FACE_BACK:
            rotated = mcCube_rotateCubeZ(rotated);
            sequence = incr_byte(sequence, 0);
          case MC_CUBE_FACE_RIGHT:
            rotated = mcCube_rotateCubeZ(rotated);
            sequence = incr_byte(sequence, 0);
          case MC_CUBE_FACE_FRONT:
            break;
          case MC_CUBE_FACE_BOTTOM:
            rotated = mcCube_rotateCubeX(rotated);
            rotated = mcCube_rotateCubeX(rotated);
            sequence = incr_byte(sequence, 1);
            sequence = incr_byte(sequence, 1);
          case MC_CUBE_FACE_TOP:
            rotated = mcCube_rotateCubeX(rotated);
            sequence = incr_byte(sequence, 1);
            break;
        }
        /* Rotate about the Y axis four times */
        for (int j = 0; j < 4; ++j) {
          if (table[rotated] != -1) {
            if (canonical == -1) {
              /* We found the canonical orientation for this cube */
              canonical = table[rotated];
              table[cell] = canonical;
            } else {
              assert(table[rotated] == canonical);
            }
          }
          if (rotated == canonical) {
            /* We found the rotation sequence that brings us into the canonical
             * orientation for this cell */
            /* Store the rotation sequence and inversion flag */
            sequenceTable[cell] = sequence;
          }
          rotated = mcCube_rotateCubeY(rotated);
          sequence = incr_byte(sequence, 2);
        }
      }
      if (mcCube_hasAmbiguousFace(cell)) {
        /* Don't invert cell configurations that have ambiguous faces */
        break;
      }
      /* Invert the cell */
      inverted = ~inverted & 0xff;
    }
    if (canonical == -1) {
      /* We could not find the current cell configuration in the table, so this
       * cell configuration defines a canonical cell orientation+inversion */
      canonical = cell;
      list[listIndex++] = canonical;
      table[cell] = canonical;
      sequenceTable[cell] = 0;
    }
  }

  assert(listIndex == MC_TRANSVOXEL_NUM_CANONICAL_REGULAR_CELLS);
}

int mcTransvoxel_getTransitionCellFace(int cell, int index) {
  int result = 0;
  switch (index) {
    case 0:
      /* The low resolution face */
      result |= (cell & (1 << 0)) ? (1 << 0) : 0;
      result |= (cell & (1 << 2)) ? (1 << 1) : 0;
      result |= (cell & (1 << 6)) ? (1 << 2) : 0;
      result |= (cell & (1 << 8)) ? (1 << 3) : 0;
      break;
    case 1:
      /* The top-left high-resolution face */
      result |= (cell & (1 << 0)) ? (1 << 0) : 0;
      result |= (cell & (1 << 1)) ? (1 << 1) : 0;
      result |= (cell & (1 << 3)) ? (1 << 2) : 0;
      result |= (cell & (1 << 4)) ? (1 << 3) : 0;
      break;
    case 2:
      /* The top-right high-resolution face */
      result |= (cell & (1 << 1)) ? (1 << 0) : 0;
      result |= (cell & (1 << 2)) ? (1 << 1) : 0;
      result |= (cell & (1 << 4)) ? (1 << 2) : 0;
      result |= (cell & (1 << 5)) ? (1 << 3) : 0;
      break;
    case 3:
      /* The bottom-left high-resolution face */
      result |= (cell & (1 << 3)) ? (1 << 0) : 0;
      result |= (cell & (1 << 4)) ? (1 << 1) : 0;
      result |= (cell & (1 << 6)) ? (1 << 2) : 0;
      result |= (cell & (1 << 7)) ? (1 << 3) : 0;
      break;
    case 4:
      /* The bottom-right high-resolution face */
      result |= (cell & (1 << 4)) ? (1 << 0) : 0;
      result |= (cell & (1 << 5)) ? (1 << 1) : 0;
      result |= (cell & (1 << 7)) ? (1 << 2) : 0;
      result |= (cell & (1 << 8)) ? (1 << 3) : 0;
      break;
    default:
      assert(0);
  }
  return result;
}

int mcTransvoxel_isAmbiguousFace(int face) {
  switch (face) {
    case 0x6:
    case 0x9:
      return 1;
  }
  return 0;
}

int mcTransvoxel_transitionCellHasAmbiguousFace(int cell) {
  /* Check the low resolution face and high resolution faces for the ambiguous
   * cases */
  for (int i = 0; i < 5; ++i) {
    int face = mcTransvoxel_getTransitionCellFace(cell, i);
    if (mcTransvoxel_isAmbiguousFace(face)) {
      return 1;
    }
  }
  return 0;
}

void mcTransvoxel_computeTransitionCellEdgeIntersectionTable() {
  for (int cell = 0; cell <= 0x1ff; ++cell) {
  }
}

void mcTransvoxel_printTransitionCell(int cell) {
  fprintf(stderr,
      "cell: 0x%03x\n"
      "%s----%s----%s\n"
      "|    |    |\n"
      "%s----%s----%s\n"
      "|    |    |\n"
      "%s----%s----%s\n\n",
      cell,
      cell & (1 << 0) ? "@" : ".",
      cell & (1 << 1) ? "@" : ".",
      cell & (1 << 2) ? "@" : ".",
      cell & (1 << 3) ? "@" : ".",
      cell & (1 << 4) ? "@" : ".",
      cell & (1 << 5) ? "@" : ".",
      cell & (1 << 6) ? "@" : ".",
      cell & (1 << 7) ? "@" : ".",
      cell & (1 << 8) ? "@" : ".");
}

void mcTransvoxel_computeCanonicalTransitionCellTable(
    int *list,
    int *table,
    int *sequenceTable)
{
  memset(table, -1, sizeof(int) * 512);
  memset(sequenceTable, -1, sizeof(int) * 512);
  int listIndex = 0;

  /* Iterate over all transition cell configurations */
  for (int cell = 0; cell <= 0x1ff; ++cell) {
    int canonical = -1;
    /* Loop to invert the transition cell */
    int inverted = cell;
    int sequence = 0;
    for (int i = 0; i < 2; ++i) {
      /* Loop to mirror the transition cell */
      int mirrored = inverted;
      sequence = set_byte(sequence, 1, 0);
      for (int j = 0; j < 2; ++j) {
        /* Loop to rotate the transition cell four times */
        int rotated = mirrored;
        fprintf(stderr, "sequence before clear rotate byte: 0x%08x\n",
            sequence);
        sequence = set_byte(sequence, 0, 0);
        fprintf(stderr, "sequence after clear rotate byte: 0x%08x\n",
            sequence);
        for (int k = 0; k < 4; ++k) {
          /* Check if this orientation has already been considered */
          if (table[rotated] != -1) {
            if (canonical == -1) {
              /* We found the canonical orientation for this cell */
              canonical = table[rotated];
              table[cell] = canonical;
            } else {
              /* All entries in the table must agree with our canonical */
              assert(table[rotated] == canonical);
            }
          }
          if (rotated == canonical) {
            /* We found the rotation+inversion sequence that brings this cell to
             * the canonical orientation. */
            if (sequenceTable[cell] == -1) {
              /* Store this rotation+inversion sequence */
              sequenceTable[cell] = sequence;
            }
          }
          /* Rotate and increment the rotation byte */
          rotated = mcTransvoxel_rotateTransitionCell(rotated);
          fprintf(stderr, "sequence before rotate: 0x%08x\n",
              sequence);
          sequence = incr_byte(sequence, 0);
          fprintf(stderr, "sequence after rotate: 0x%08x\n",
              sequence);
        }
        /* Reflect and set the mirrored byte */
        mirrored = mcTransvoxel_reflectTransitionCell(mirrored);
        sequence = incr_byte(sequence, 1);
      }
      if (mcTransvoxel_transitionCellHasAmbiguousFace(cell)) {
        /* Don't invert ambigous faces */
        break;
      }
      /* Invert and set the inversion byte */
      inverted = mcTransvoxel_invertTransitionCell(inverted);
      sequence = incr_byte(sequence, 2);
    }
    if (canonical == -1) {
      /* We could not find the current cell configuration in the table, so this
       * cell configuration defines a canonical cell orientation+inversion */
      canonical = cell;
      list[listIndex++] = canonical;
      table[cell] = canonical;
      sequenceTable[cell] = 0;
      mcTransvoxel_printTransitionCell(canonical);
    }
    assert(sequenceTable[cell] != -1);
  }

  assert(listIndex == MC_TRANSVOXEL_NUM_CANONICAL_TRANSITION_CELLS);
}

void mcTransvoxel_printRegularCellTable(const int *table, FILE *fp) {
  /* Iterate over all regular cell configurations and print the table */
  for (int i = 0; i <= 0xff; i += 8) {
    fprintf(fp, "  ");
    for (int j = 0; j < 8; ++j) {
      int cell = i + j;
      fprintf(fp, "0x%02x,", table[cell]);
      if (j == 7)
        fprintf(fp, "\n");
      else
        fprintf(fp, " ");
    }
  }
}

void mcTransvoxel_printTransitionCellTable(const int *table, FILE *fp) {
  /* Iterate over all transition cell configurations and print the table */
  for (int i = 0; i <= 0x1ff; i += 8) {
    fprintf(fp, "  ");
    for (int j = 0; j < 8; ++j) {
      int cell = i + j;
      fprintf(fp, "0x%03x,", table[cell]);
      if (j == 7)
        fprintf(fp, "\n");
      else
        fprintf(fp, " ");
    }
  }
}

void mcTransvoxel_printCanonicalRegularCellTable(int *table, FILE *fp) {
  fprintf(fp, "int mcTransvoxel_canonicalRegularCellTable[] = {\n");
  mcTransvoxel_printRegularCellTable(table, fp);
  fprintf(fp, "};\n");
}

void mcTransvoxel_printCanonicalTransitionCellTable(int *table, FILE *fp) {
  fprintf(fp, "int mcTransvoxel_canonicalTransitionCellTable[] = {\n");
  mcTransvoxel_printTransitionCellTable(table, fp);
  fprintf(fp, "};\n");
}

void mcTransvoxel_printCanonicalRegularCellSequenceTable(
    int *table, FILE *fp)
{
  fprintf(fp, "int mcTransvoxel_canonicalRegularCellSequenceTable[] = {\n");
  /* Iterate over all transition cell configurations and print the table */
  for (int i = 0; i <= 0xff; i += 4) {
    fprintf(fp, "  ");
    for (int j = 0; j < 4; ++j) {
      int cell = i + j;
      fprintf(fp, "0x%08x,", table[cell]);
      if (j == 3)
        fprintf(fp, "\n");
      else
        fprintf(fp, " ");
    }
  }
  fprintf(fp, "};\n");
}

void mcTransvoxel_printCanonicalTransitionCellSequenceTable(
    int *table, FILE *fp)
{
  fprintf(fp, "int mcTransvoxel_canonicalTransitionCellSequenceTable[] = {\n");
  /* Iterate over all transition cell configurations and print the table */
  for (int i = 0; i <= 0x1ff; i += 4) {
    fprintf(fp, "  ");
    for (int j = 0; j < 4; ++j) {
      int cell = i + j;
      fprintf(fp, "0x%08x,", table[cell]);
      if (j == 3)
        fprintf(fp, "\n");
      else
        fprintf(fp, " ");
    }
  }
  fprintf(fp, "};\n");
}

void mcTransvoxel_printCanonicalRegularCellList(
    int *list, FILE *fp)
{
  fprintf(fp, "typedef enum {\n");
  for (int i = 0; i < MC_TRANSVOXEL_NUM_CANONICAL_REGULAR_CELLS; ++i) {
    fprintf(fp,
        "  MC_TRANSVOXEL_CANONICAL_REGULAR_CELL_%d = 0x%03x,\n",
        i, list[i]);
  }
  fprintf(fp, "} mcTransvoxel_CanonicalRegularCell;\n");
}

void mcTransvoxel_printCanonicalTransitionCellList(
    int *list, FILE *fp)
{
  fprintf(fp, "typedef enum {\n");
  for (int i = 0; i < MC_TRANSVOXEL_NUM_CANONICAL_TRANSITION_CELLS; ++i) {
    fprintf(fp,
        "  MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_%d = 0x%03x,\n",
        i, list[i]);
  }
  fprintf(fp, "} mcTransvoxel_CanonicalTransitionCell;\n");
}

void print_usage() {
  fprintf(stderr,
      "Usage:\n"
      "transvoxel_generate_transform_tables [filename]\n\n"
      "Where [filename] is one of the following:\n"
      "    transvoxel_canonical_cell_tables.c\n"
      "    transvoxel_canonical_cells.h\n\n"
      );
}

int main(int argc, char **argv) {
  typedef enum {
    TRANSVOXEL_CANONICAL_CELL_TABLES_C,
    TRANSVOXEL_CANONICAL_CELLS_H,
  } OutputFile;

  OutputFile output;
  int *canonicalRegularCellList,
      *canonicalRegularCellTable,
      *canonicalRegularCellSequenceTable,
      *canonicalTransitionCellList,
      *canonicalTransitionCellTable,
      *canonicalTransitionCellSequenceTable;

  /* Parse the arguments to determine which file we are generating */
  if (argc != 2) {
    print_usage();
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "transvoxel_canonical_cell_tables.c") == 0) {
    output = TRANSVOXEL_CANONICAL_CELL_TABLES_C;
  } else if (strcmp(argv[1], "transvoxel_canonical_cells.h") == 0) {
    output = TRANSVOXEL_CANONICAL_CELLS_H;
  } else {
    print_usage();
    return EXIT_FAILURE;
  }

  /* Allocate memory for each table */
  canonicalRegularCellList = (int*)malloc(
      sizeof(int) * MC_TRANSVOXEL_NUM_REGULAR_CELLS);
  canonicalRegularCellTable = (int*)malloc(sizeof(int) * 512);
  canonicalRegularCellSequenceTable = (int*)malloc(sizeof(int) * 512);
  canonicalTransitionCellList = (int*)malloc(
      sizeof(int) * MC_TRANSVOXEL_NUM_TRANSITION_CELLS);
  canonicalTransitionCellTable = (int*)malloc(sizeof(int) * 512);
  canonicalTransitionCellSequenceTable = (int*)malloc(sizeof(int) * 512);

  /* Compute the tables */
  mcTransvoxel_computeCanonicalRegularCellTable(
      canonicalRegularCellList,
      canonicalRegularCellTable,
      canonicalRegularCellSequenceTable);
  mcTransvoxel_computeCanonicalTransitionCellTable(
      canonicalTransitionCellList,
      canonicalTransitionCellTable,
      canonicalTransitionCellSequenceTable);

  /* Print the tables */
  /* NOTE: stdout is used because Emscripten's filesystem model makes using
   * fopen() difficult */
  /* NOTE: Some computation might be wasted because we need to run this program
   * once for each file generated. This is acceptable since our compile times
   * mostly depend on the performance of Emscripten's Javascript optimizer. */
  switch (output) {
    case TRANSVOXEL_CANONICAL_CELL_TABLES_C:
      mcTransvoxel_printCanonicalRegularCellTable(
          canonicalRegularCellTable, stdout);
      fprintf(stdout, "\n");
      mcTransvoxel_printCanonicalRegularCellSequenceTable(
          canonicalRegularCellSequenceTable, stdout);
      fprintf(stdout, "\n");
      mcTransvoxel_printCanonicalTransitionCellTable(
          canonicalTransitionCellTable, stdout);
      fprintf(stdout, "\n");
      mcTransvoxel_printCanonicalTransitionCellSequenceTable(
          canonicalTransitionCellSequenceTable, stdout);
      break;
    case TRANSVOXEL_CANONICAL_CELLS_H:
      mcTransvoxel_printCanonicalRegularCellList(
          canonicalRegularCellList, stdout);
      fprintf(stdout, "\n");
      mcTransvoxel_printCanonicalTransitionCellList(
          canonicalTransitionCellList, stdout);
      break;
  }

  /* Free table memory */
  free(canonicalTransitionCellSequenceTable);
  free(canonicalTransitionCellTable);
  free(canonicalTransitionCellList);
  free(canonicalRegularCellSequenceTable);
  free(canonicalRegularCellTable);
  free(canonicalRegularCellList);
}
