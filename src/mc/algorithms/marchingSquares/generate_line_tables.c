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

#include <mc/algorithms/common/square.h>
#include <mc/algorithms/marchingSquares/canonical.h>
#include <mc/algorithms/marchingSquares/common.h>

#define get_byte(num, byte) (((num) & (0xff << (8 * byte))) >> (8 * byte))

#include "marching_squares_canonical_squares.h"

void mcMarchingSquares_calculateLineTable(
    mcMarchingSquares_LineList *table)
{
  /* Iterate over all possible square configurations */
  for (int square = 0; square <= 0xf; ++square) {
    int canonical, sequence, listIndex;
    mcMarchingSquares_LineList *list;
    canonical = mcMarchingSquares_canonicalSquare(square);
    sequence = mcMarchingSquares_canonicalSquareSequence(square);
    list = &table[square];
    listIndex = 0;

    /* Initialize the line list to empty */
    memset(list, -1, sizeof(mcMarchingSquares_LineList));

    /* Generate lines for the canonical square configuration */
#define MAKE_LINE(alpha, beta) \
    do { \
      list->lines[listIndex].a = (alpha); \
      list->lines[listIndex].b = (beta); \
      listIndex += 1; \
    } while (0)
    switch (canonical) {
      case MC_MARCHING_SQUARES_CANONICAL_SQUARE_0:
        /* This is the trivial case with no samples inside the contour */
        break;
      case MC_MARCHING_SQUARES_CANONICAL_SQUARE_1:
        /* This case has one sample inside the contour, which generates one
         * line on the corner of the square */
        MAKE_LINE(0, 3);
        break;
      case MC_MARCHING_SQUARES_CANONICAL_SQUARE_2:
        /* This case has two samples on the same square edge inside the
         * contour, which generates a single line through the center of the
         * square. */
        MAKE_LINE(1, 3);
        break;
      case MC_MARCHING_SQUARES_CANONICAL_SQUARE_3:
        /* This case has two samples diagonal from each other inside the
         * contour. This happens to be an ambiguous case in which we
         * arbitrarily decide to split the contour rather than join the samples
         * inside the contour. */
        MAKE_LINE(1, 0);
        MAKE_LINE(3, 2);
        break;
    }
    /* Rotate the canonical lines into their final positions */
    for (int i = 0; i < MC_MARCHING_SQUARES_MAX_NUM_LINES; ++i) {
      mcLine *line = &list->lines[i];
      if (line->a == -1)
        break;  /* No more lines to consider */
      /* Rotate each edge intersection in the line */
#define ROTATE_LINE_EDGE_INTERSECTION(edge) \
      for (int j = 0; j < get_byte(sequence, 0); ++j) { \
        line->edge = mcSquare_rotateEdgeReverse(line->edge); \
      }
      ROTATE_LINE_EDGE_INTERSECTION(a)
      ROTATE_LINE_EDGE_INTERSECTION(b)
      /* Square inversion reverses line winding order */
      if (get_byte(sequence, 1)) {
        int temp = line->a;
        line->a = line->b;
        line->b = temp;
      }
    }
  }
}

void mcMarchingSquares_printLineTable(
    mcMarchingSquares_LineList *table,
    FILE *fp)
{
  fprintf(fp,
      "mcMarchingSquares_LineList\n"
      "mcMarchingSquares_lineTable[] = {\n");
  /* Iterate over all possible square configurations */
  for (int square = 0; square <= 0xf; ++square) {
    /* Print the list of lines for this square configuration */
    fprintf(fp,
        "  {\n"
        "    .lines = {\n");
    for (int i = 0; i < 2; ++i) {
      fprintf(fp,
          "      {\n"
          "        .a = %d,\n"
          "        .b = %d,\n"
          "      },\n",
          table[square].lines[i].a,
          table[square].lines[i].b);
    }
    fprintf(fp, 
        "    },\n"
        "  },\n");
  }
  fprintf(fp, "};\n");
}

void print_usage() {
  fprintf(stderr,
      "Usage:\n"
      "marching_squares_generate_line_tables [filename]\n\n"
      "Where [filename] is one of the following:\n"
      "    marching_squares_line_tables.c\n"
      );
}

int main(int argc, char **argv) {
  enum {
    MARCHING_SQUARES_LINE_TABLES_C,
  } output;

  mcMarchingSquares_LineList *lineTable;

  /* Parse the arguments to determine which file we are generating */
  if (argc != 2) {
    print_usage();
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "marching_squares_line_tables.c") == 0) {
    output = MARCHING_SQUARES_LINE_TABLES_C;
  } else {
    print_usage();
    return EXIT_FAILURE;
  }

  /* Allocate memory for each of our tables */
  lineTable = (mcMarchingSquares_LineList*)malloc(
      sizeof(mcMarchingSquares_LineList) * 16);

  /* Compute the tables */
  mcMarchingSquares_calculateLineTable(lineTable);

  /* Print the tables */
  switch (output) {
    case MARCHING_SQUARES_LINE_TABLES_C:
      mcMarchingSquares_printLineTable(lineTable, stdout);
      break;
  }

  free(lineTable);

  return EXIT_SUCCESS;
}
