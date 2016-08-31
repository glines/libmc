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

#include <mc/algorithms/coloredMarchingSquares/canonical.h>
#include <mc/algorithms/coloredMarchingSquares/common.h>
#include <mc/algorithms/common/square.h>

#include "colored_marching_squares_canonical.h"

#define get_byte(num, byte) (((num) & (0xff << (8 * byte))) >> (8 * byte))

void mcColoredMarchingSquares_calculateLineTable(
    mcColoredMarchingSquares_LineList *table)
{
  /* Iterate over all possible square configurations */
  for (int square = 0;
      square < MC_COLORED_MARCHING_SQUARES_NUM_SQUARES;
      ++square)
  {
    int listIndex, canonical, sequence;
    mcColoredMarchingSquares_LineList *list;
    canonical = mcColoredMarchingSquares_canonicalSquare(square);
    sequence = mcColoredMarchingSquares_canonicalSquareSequence(square);
    list = &table[square];
    listIndex = 0;

    /* Initialize the line list to empty */
    memset(list, -1, sizeof(mcColoredMarchingSquares_LineList));

    /* Generate lines for the canonical square configuration */
#define MAKE_LINE(alpha, beta) \
    do { \
      list->lines[listIndex].a = (alpha); \
      list->lines[listIndex].b = (beta); \
      listIndex += 1; \
    } while(0)
    switch (canonical) {
      case MC_COLORED_MARCHING_SQUARES_CANONICAL_SQUARE_0:
        /* This is the trivial case where all corners are the same color */
        break;
      case MC_COLORED_MARCHING_SQUARES_CANONICAL_SQUARE_1:
        /* This case has a single corner a different color from the others.
         * This generates one line. */
        MAKE_LINE(0, 3);
        break;
      case MC_COLORED_MARCHING_SQUARES_CANONICAL_SQUARE_2:
        /* This case has half of the square one color, and half of the square
         * another color. A single line through the center of the square is
         * generated. */
        MAKE_LINE(1, 3);
        break;
    }
    /* Transform the generated lines into their final positions */
    for (int i = 0; i < MC_COLORED_MARCHING_SQUARES_MAX_NUM_LINES; ++i) {
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
      /* Mirror each edge intersection in the line */
      if (get_byte(sequence, 1)) {
        line->a = mcSquare_mirrorEdge(line->a);
        line->b = mcSquare_mirrorEdge(line->b);
      }
    }
  }
}

void mcColoredMarchingSquares_printLineTable(
    mcColoredMarchingSquares_LineList *table,
    FILE *fp)
{  fprintf(fp,
      "mcColoredMarchingSquares_LineList\n"
      "mcColoredMarchingSquares_lineTable[] = {\n");
  /* Iterate over all possible square configurations */
  for (int square = 0;
      square <= MC_COLORED_MARCHING_SQUARES_NUM_SQUARES;
      ++square)
  {
    /* Print the list of lines for this square configuration */
    fprintf(fp,
        "  {\n"
        "    .lines = {\n");
    for (int i = 0;
        i < MC_COLORED_MARCHING_SQUARES_MAX_NUM_LINES;
        ++i)
    {
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
      "colored_marching_squares_generate_line_tables [filename]\n\n"
      "Where [filename] is one of the following:\n"
      "    colored_marching_squares_line_tables.c\n"
      );
}

int main(int argc, char **argv) {
  enum {
    COLORED_MARCHING_SQUARES_LINE_TABLES_C,
  } output;

  mcColoredMarchingSquares_LineList *lineTable;

  /* Parse the arguments to determine which file we are generating */
  if (argc != 2) {
    print_usage();
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "colored_marching_squares_line_tables.c") == 0) {
    output = COLORED_MARCHING_SQUARES_LINE_TABLES_C;
  } else {
    print_usage();
    return EXIT_FAILURE;
  }

  /* Allocate memory for each of our tables */
  lineTable = (mcColoredMarchingSquares_LineList*)malloc(
      sizeof(mcColoredMarchingSquares_LineList)
      * MC_COLORED_MARCHING_SQUARES_NUM_SQUARES);

  /* Compute the tables */
  mcColoredMarchingSquares_calculateLineTable(lineTable);

  /* Print the tables */
  switch (output) {
    case COLORED_MARCHING_SQUARES_LINE_TABLES_C:
      mcColoredMarchingSquares_printLineTable(lineTable, stdout);
      break;
  }
}
