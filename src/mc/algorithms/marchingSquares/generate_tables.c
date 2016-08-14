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

#include <mc/algorithms/marchingSquares/common.h>
#include <mc/algorithms/common/square.h>

#define get_byte(num, byte) (((num) & (0xff << (8 * byte))) >> (8 * byte))
#define set_byte(num, byte, val) (((num) & ~(0xff << (8 * byte))) | ((val & 0xff) << (8 * byte)))
#define incr_byte(num, byte) (set_byte(num, byte, get_byte(num, byte) + 1))

void mcMarchingSquares_computeEdgeIntersectionTable(
    mcMarchingSquares_EdgeIntersectionList *table)
{
  /* Iterate over all possible square configurations */
  for (int square = 0; square <= 0xf; ++square) {
    int *edgeIntersections = table[square].edges;
    int edgeIntersectionIndex = 0;
    /* Initialize the edge intersections with -1 */
    memset(edgeIntersections, -1, sizeof(int) * 4);
    /* Iterate over all four edges */
    for (int edge = 0; edge < 4; ++edge) {
      /* Check the samples on this edge */
      int sampleIndices[2];
      int samples[2];
      mcSquare_edgeSampleIndices(edge, sampleIndices);
      for (int i = 0; i < 2; ++i) {
        samples[i] = mcSquare_sampleValue(square, sampleIndices[i]);
      }
      if (samples[0] != samples[1]) {
        /* Samples on opposite sides of the isoline indicate an edge
         * intersection */
        edgeIntersections[edgeIntersectionIndex++] = edge;
      }
    }
  }
}

void mcMarchingSquares_computeCanonicalSquareTable(
    int *list,
    int *table,
    int *sequenceTable)
{
  unsigned int listIndex;

  /* Initialize all tables */
  memset(table, -1, sizeof(int) * 16);
  memset(sequenceTable, -1, sizeof(int) * 16);
  listIndex = 0;

  /* Iterate over all possible square configurations */
  for (int square = 0; square <= 0xf; ++square) {
    int canonical = -1;
    /* Loop to invert the square */
    int inverted = square;
    for (int i = 0; i < 2; ++i) {
      int sequence;
      /* Set the inversion byte */
      sequence = set_byte(0, 1, i);
      int rotated = inverted;
      /* Loop to rotate the square */
      for (int j = 0; j < 4; ++j) {
        /* TODO: Check for canonical square? */
        if (table[rotated] != -1) {
          if (canonical == -1) {
            /* We found the canonical orientation for this square */
            canonical = table[rotated];
            table[square] = canonical;
          } else {
            assert(table[rotated] == canonical);
          }
        }
        if (rotated == canonical) {
          /* We found the rotation sequence that brings this square into the
           * canonical orientation */
          /* Store the rotation sequence and inversion flag */
          sequenceTable[square] = sequence;
        }
        /* Rotate the square */
        rotated = mcSquare_rotateSquare(rotated);
        /* Update the rotation byte */
        sequence = incr_byte(sequence, 0);
      }
      /* Invert the square */
      inverted = mcSquare_invertSquare(inverted);
    }
    if (canonical == -1) {
      /* We could not find the current square configuration in the table, so
       * this square configuration defines a canonical square */
      canonical = square;
      list[listIndex++] = canonical;
      table[square] = canonical;
      sequenceTable[square] = 0;
    }
  }

  assert(listIndex == MC_MARCHING_SQUARES_NUM_CANONICAL_SQUARES);
}

void mcMarchingSquares_computeCanonicalSquares() {
}

void mcMarchingSquares_printEdgeIntersectionTable(
    const mcMarchingSquares_EdgeIntersectionList *table,
    FILE *fh)
{
  fprintf(fh,
      "mcMarchingSquares_EdgeIntersectionList\n"
      "mcMarchingSquares_edgeIntersectionTable[] = {\n");
  /* Iterate over all possible square configurations */
  for (int square = 0; square <= 0xf; ++square) {
    /* Print out the edge intersection list for this square */
    fprintf(fh, "  {\n");
    fprintf(fh, "    .edges = { ");
    for (int i = 0; i < 4; ++i) {
      fprintf(fh, "%2d, ", table[square].edges[i]);
    }
    fprintf(fh, "}\n");
    fprintf(fh, "  },\n");
  }
  fprintf(fh, "};\n");
}

void mcMarchingSquares_printCanonicalSquareTable(
    const int *table, FILE *fh)
{
  fprintf(fh, "const int mcMarchingSquares_canonicalSquareTable[] = {\n");
  /* Iterate over all possible square configurations */
  for (int square = 0; square <= 0xf; ++square) {
    if (square % 8 == 0) {
      fprintf(fh, "  ");
    }
    fprintf(fh, "0x%x, ", table[square]);
    if ((square + 1) % 8 == 0) {
      fprintf(fh, "\n");
    }
  }
  fprintf(fh, "};\n");
}

void mcMarchingSquares_printCanonicalSquareSequenceTable(
    const int *table, FILE *fh)
{
  fprintf(fh, "const int mcMarchingSquares_canonicalSquareSequenceTable[] = {\n");
  /* Iterate over all possible square configurations */
  for (int square = 0; square <= 0xf; ++square) {
    if (square % 8 == 0) {
      fprintf(fh, "  ");
    }
    fprintf(fh, "0x%04x, ", table[square]);
    if ((square + 1) % 8 == 0) {
      fprintf(fh, "\n");
    }
  }
  fprintf(fh, "};\n");
}

void mcMarchingSquares_printCanonicalSquareList(
    const int *list,
    FILE *fh)
{
  fprintf(fh, "typedef enum {\n");
  /* Iterate over all canonical square configurations */
  for (int i = 0; i < MC_MARCHING_SQUARES_NUM_CANONICAL_SQUARES; ++i) {
    fprintf(fh, "  MC_MARCHING_SQUARES_CANONICAL_SQUARE_%d = 0x%x,\n",
        i, list[i]);
  }
  fprintf(fh, "} mcMarchingSquares_CanonicalSquare;\n");
}

void print_usage() {
  fprintf(stderr,
      "Usage:\n"
      "marching_squares_generate_tables [filename]\n\n"
      "Where [filename] is one of the following:\n"
      "    marching_squares_tables.c\n"
      "    marching_squares_canonical_squares.h\n"
      );
}

int main(int argc, char **argv) {
  enum {
    MARCHING_SQUARES_TABLES_C,
    MARCHING_SQUARES_CANONICAL_SQUARES_H,
  } output;

  mcMarchingSquares_EdgeIntersectionList *edgeIntersectionTable;
  int *canonicalSquareList,
      *canonicalSquareTable,
      *canonicalSquareSequenceTable;
  mcMarchingSquares_LineList *lineTable;

  /* Parse the arguments to determine which file we are generating */
  if (argc != 2) {
    print_usage();
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "marching_squares_tables.c") == 0) {
    output = MARCHING_SQUARES_TABLES_C;
  } else if (strcmp(argv[1], "marching_squares_canonical_squares.h") == 0) {
    output = MARCHING_SQUARES_CANONICAL_SQUARES_H;
  } else {
    print_usage();
    return EXIT_FAILURE;
  }

  /* Allocate memory for each table */
  edgeIntersectionTable =
    (mcMarchingSquares_EdgeIntersectionList*)malloc(
        sizeof(mcMarchingSquares_EdgeIntersectionList) * 16);
  canonicalSquareList = (int*)malloc(
      sizeof(int) * MC_MARCHING_SQUARES_NUM_CANONICAL_SQUARES);
  canonicalSquareTable = (int*)malloc(sizeof(int) * 16);
  canonicalSquareSequenceTable = (int*)malloc(sizeof(int) * 16);

  /* Compute each table */
  mcMarchingSquares_computeEdgeIntersectionTable(edgeIntersectionTable);
  mcMarchingSquares_computeCanonicalSquareTable(
      canonicalSquareList,
      canonicalSquareTable,
      canonicalSquareSequenceTable);

  /* Print out all the tables */
  switch (output) {
    case MARCHING_SQUARES_TABLES_C:
      mcMarchingSquares_printEdgeIntersectionTable(
          edgeIntersectionTable, stdout);
      fprintf(stdout, "\n");
      mcMarchingSquares_printCanonicalSquareTable(
          canonicalSquareTable, stdout);
      fprintf(stdout, "\n");
      mcMarchingSquares_printCanonicalSquareSequenceTable(
          canonicalSquareSequenceTable, stdout);
      break;
    case MARCHING_SQUARES_CANONICAL_SQUARES_H:
      mcMarchingSquares_printCanonicalSquareList(
          canonicalSquareList, stdout);
      break;
  }

  /* Free table memory */
  free(canonicalSquareSequenceTable);
  free(canonicalSquareTable);
  free(canonicalSquareList);
  free(edgeIntersectionTable);

  return EXIT_SUCCESS;
}
