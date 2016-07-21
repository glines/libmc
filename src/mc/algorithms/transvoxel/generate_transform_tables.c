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
#include <string.h>

#include <mc/algorithms/transvoxel/common.h>

void mcTransvoxel_computeTransitionCellRotationTable(int *table) {
  /* Iterate over all transition cell configurations */
  for (int cell = 0; cell <= 0x1ff; ++cell) {
    /* Rotate this cell counterclockwise */
    int rotated = 0;
    rotated |= (cell & (1 << 0)) ? (1 << 2) : 0;
    rotated |= (cell & (1 << 1)) ? (1 << 5) : 0;
    rotated |= (cell & (1 << 2)) ? (1 << 8) : 0;
    rotated |= (cell & (1 << 3)) ? (1 << 1) : 0;
    rotated |= (cell & (1 << 4)) ? (1 << 4) : 0;
    rotated |= (cell & (1 << 5)) ? (1 << 7) : 0;
    rotated |= (cell & (1 << 6)) ? (1 << 0) : 0;
    rotated |= (cell & (1 << 7)) ? (1 << 3) : 0;
    rotated |= (cell & (1 << 8)) ? (1 << 6) : 0;
    table[cell] = rotated;
  }
}

void mcTransvoxel_computeTransitionCellReflectionTable(int *table) {
  /* Iterate over all transition cell configurations */
  for (int cell = 0; cell <= 0x1ff; ++cell) {
    /* Reflect this cell */
    /* NOTE: It is not important to the algorithm which axis we reflect the
     * cell about, as long as we catch the mirror image of any asymmetrical
     * cells. */
    int reflected = 0;
    reflected |= (cell & (1 << 0)) ? (1 << 2) : 0;
    reflected |= cell & (1 << 1);
    reflected |= (cell & (1 << 2)) ? (1 << 0) : 0;
    reflected |= (cell & (1 << 3)) ? (1 << 5) : 0;
    reflected |= cell & (1 << 4);
    reflected |= (cell & (1 << 5)) ? (1 << 3) : 0;
    reflected |= (cell & (1 << 6)) ? (1 << 8) : 0;
    reflected |= cell & (1 << 7);
    reflected |= (cell & (1 << 8)) ? (1 << 6) : 0;
    table[cell] = reflected;
  }
}

void mcTransvoxel_computeTransitionCellEdgeRotationTable(int *table) {
  /* NOTE: This routine does not actually "compute" a table. It is included for
   * consistency, since we do actually compute the reverse table. */
  /* Iterate over all transition cell edges */
  static const int rotationTable[] = {
    1,   /* Edge 0 */
    2,   /* Edge 1 */
    3,   /* Edge 2 */
    0,   /* Edge 3 */
    12,  /* Edge 4 */
    15,  /* Edge 5 */
    11,  /* Edge 6 */
    14,  /* Edge 7 */
    10,  /* Edge 8 */
    13,  /* Edge 9 */
    5,   /* Edge 10 */
    7,   /* Edge 11 */
    9,   /* Edge 12 */
    4,   /* Edge 13 */
    6,   /* Edge 14 */
    8,   /* Edge 15 */
  };
  memcpy(table, rotationTable, sizeof(rotationTable));
}

void mcTransvoxel_computeTransitionCellEdgeReverseRotationTable(
    const int *rotationTable,
    int *table)
{
  for (int i = 0; i < MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES; ++i) {
    table[i] = i;
    for (int j = 0; j < 3; ++j) {
      /* Three rotations gives us the reverse rotation */
      table[i] = rotationTable[table[i]];
    }
  }
}

void mcTransvoxel_printTransitionCellRotationTable(
    int *table, FILE *fp)
{
  fprintf(fp, "int mcTransvoxel_transitionCellRotationTable[] = {\n");
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
  fprintf(fp, "};\n");
}

void mcTransvoxel_printTransitionCellReflectionTable(
    int *table, FILE *fp)
{
  fprintf(fp, "int mcTransvoxel_transitionCellReflectionTable[] = {\n");
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
  fprintf(fp, "};\n");
}

void mcTransvoxel_printTransitionCellEdgeRotationTable(
    int *table, FILE *fp)
{
  fprintf(fp,
      "int mcTransvoxel_transitionCellEdgeRotationTable[] = {\n");
  /* Iterate over all transition edges and print the table */
  for (int i = 0; i < MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES; ++i) {
    fprintf(fp, "  %2d,  /* Edge %d */\n",
        table[i], i);
  }
  fprintf(fp, "};\n");
}

void mcTransvoxel_printTransitionCellEdgeReverseRotationTable(
    int *table, FILE *fp)
{
  fprintf(fp,
      "int mcTransvoxel_transitionCellEdgeReverseRotationTable[] = {\n");
  /* Iterate over all transition edges and print the table */
  for (int i = 0; i < MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES; ++i) {
    fprintf(fp, "  %2d,  /* Edge %d */\n",
        table[i], i);
  }
  fprintf(fp, "};\n");
}

void print_usage() {
  fprintf(stderr,
      "Usage:\n"
      "transvoxel_generate_transform_tables [filename]\n\n"
      "Where [filename] is one of the following:\n"
      "    transvoxel_transform_tables.c\n\n"
      );
}

int main(int argc, char **argv) {
  enum {
    TRANSVOXEL_TRANSFORM_TABLES_C,
  } output;

  int *transitionCellRotationTable,
      *transitionCellReflectionTable,
      *transitionCellEdgeRotationTable,
      *transitionCellEdgeReverseRotationTable;

  /* Parse the arguments to determine which file we are generating */
  if (argc != 2) {
    print_usage();
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "transvoxel_transform_tables.c") == 0) {
    output = TRANSVOXEL_TRANSFORM_TABLES_C;
  } else {
    print_usage();
    return EXIT_FAILURE;
  }

  /* Allocate memory for each table */
  transitionCellRotationTable = (int*)malloc(sizeof(int) * 512);
  transitionCellReflectionTable = (int*)malloc(sizeof(int) * 512);
  transitionCellEdgeRotationTable = (int*)malloc(
      sizeof(int) * MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES);
  transitionCellEdgeReverseRotationTable = (int*)malloc(
      sizeof(int) * MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES);

  /* Compute the tables */
  mcTransvoxel_computeTransitionCellRotationTable(
      transitionCellRotationTable);
  mcTransvoxel_computeTransitionCellReflectionTable(
      transitionCellReflectionTable);
  mcTransvoxel_computeTransitionCellEdgeRotationTable(
      transitionCellEdgeRotationTable);
  mcTransvoxel_computeTransitionCellEdgeReverseRotationTable(
      transitionCellEdgeRotationTable,
      transitionCellEdgeReverseRotationTable);

  /* Print the tables */
  /* NOTE: stdout is used because Emscripten's filesystem model makes using
   * fopen() difficult */
  /* NOTE: Some computation might be wasted because we need to run this program
   * once for each file generated. This is acceptable since our compile times
   * mostly depend on the performance of Emscripten's Javascript optimizer. */
  switch (output) {
    case TRANSVOXEL_TRANSFORM_TABLES_C:
      mcTransvoxel_printTransitionCellRotationTable(
          transitionCellRotationTable, stdout);
      fprintf(stdout, "\n");
      mcTransvoxel_printTransitionCellReflectionTable(
          transitionCellReflectionTable, stdout);
      fprintf(stdout, "\n");
      mcTransvoxel_printTransitionCellEdgeRotationTable(
          transitionCellEdgeRotationTable, stdout);
      fprintf(stdout, "\n");
      mcTransvoxel_printTransitionCellEdgeReverseRotationTable(
          transitionCellEdgeReverseRotationTable, stdout);
      break;
  }

  /* Free table memory */
  free(transitionCellReflectionTable);
  free(transitionCellRotationTable);
  free(transitionCellEdgeRotationTable);
  free(transitionCellEdgeReverseRotationTable);
}
