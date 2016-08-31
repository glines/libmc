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

#include <mc/algorithms/coloredMarchingSquares/common.h>
#include <mc/algorithms/common/square.h>

#define get_byte(num, byte) (((num) & (0xff << (8 * byte))) >> (8 * byte))
#define set_byte(num, byte, val) (((num) & ~(0xff << (8 * byte))) | ((val & 0xff) << (8 * byte)))
#define incr_byte(num, byte) (set_byte(num, byte, get_byte(num, byte) + 1))

void mcColoredMarchingSquares_computeEdgeIntersectionTable(
    mcColoredMarchingSquares_EdgeIntersectionList *table)
{
  /* Iterate over all possible colored square configurations */
  for (int square = 0;
      square < MC_COLORED_MARCHING_SQUARES_NUM_SQUARES;
      ++square)
  {
    int *intersections = table[square].edges;
    int intersectionIndex = 0;
    /* Initialize the edge intersections with -1 */
    memset(intersections, -1, sizeof(table[square].edges));
    /* Iterate over all four edges */
    for (int edge = 0; edge < 4; ++edge) {
      /* Check the samples on this edge */
      int sampleIndices[2];
      int samples[2];
      mcSquare_edgeSampleIndices(edge, sampleIndices);
      for (int i = 0; i < 2; ++i) {
        samples[i] = mcColoredMarchingSquares_sampleValue(
            square, sampleIndices[i]);
      }
      if (samples[0] != samples[1]) {
        /* Samples with different colors indicate an edge intersection */
        intersections[intersectionIndex++] = edge;
      }
    }
  }
}

int mcColoredMarchingSquares_normalizeColors(int square) {
  /* Each square has an intrinsic order of its colors when read from most
   * significant to least significant sample index. This routine normalizes the
   * colors such that the color order starts at zero and ascends by one with
   * each change in color. */
  int colors[4];
  memset(colors, -1, sizeof(colors));
  int colorIndex = 0;
  int normalizedSquare = 0;
#define GET_COLOR(s, i) (((s) & (0x3 << ((i) * 2))) >> ((i) * 2))
#define SET_COLOR(s, i, c) \
  (s) = ((s) & ~(0x3 << ((i) * 2))) | (((c) & 0x3) << ((i) * 2))
  for (int i = 3; i >= 0; --i) {
    if (colors[GET_COLOR(square, i)] == -1) {
      colors[GET_COLOR(square, i)] = colorIndex++;
    }
    SET_COLOR(normalizedSquare, i, colors[GET_COLOR(square, i)]);
  }
  return normalizedSquare;
}

void mcColoredMarchingSquares_computeCanonicalSquareTable(
    int *list,
    int *table,
    int *sequenceTable)
{
  int listIndex;

  /* Initialize the tables */
  memset(table, -1, sizeof(int) * MC_COLORED_MARCHING_SQUARES_NUM_SQUARES);
  memset(sequenceTable, -1, sizeof(int) * MC_COLORED_MARCHING_SQUARES_NUM_SQUARES);
  listIndex = 0;

  /* Iterate over all possible colored square configurations */
  for (int square = 0;
      square < MC_COLORED_MARCHING_SQUARES_NUM_SQUARES;
      ++square)
  {
    int canonical = -1;
    int sequence = 0;
    /* Iterate to mirror the square */
    int mirrored = square;
    for (int i = 0; i < 2; ++i) {
      /* Iterate to rotate the square in all four directions */
      int rotated = mirrored;
      for (int j = 0; j < 4; ++j) {
        /* Normalize the colors of this square */
        int normalized = mcColoredMarchingSquares_normalizeColors(rotated);
        if (table[normalized] != -1) {
          if (canonical == -1) {
            /* We found the canonical configuration for this square */
            canonical = table[normalized];
            table[square] = canonical;
          } else {
            assert(table[normalized] == canonical);
          }
        }
        if (normalized == canonical) {
          /* We found the rotation sequence that brings us into the canonical
           * orientation for this square */
          sequenceTable[square] = sequence;
        }
        /* Rotate the square */
        rotated = mcColoredMarchingSquares_rotateSquare(rotated);
        /* Update the rotation byte */
        sequence = incr_byte(sequence, 0);
      }
      /* Mirror the square */
      mirrored = mcColoredMarchingSquares_mirrorSquare(mirrored);
      /* Update the mirrored byte */
      sequence = incr_byte(sequence, 1);
    }
    if (canonical == -1) {
      /* We could not find the current square configuration in the table, so this
       * square configuration defines a canonical square */
      assert(square == mcColoredMarchingSquares_normalizeColors(square));
      canonical = square;
      list[listIndex++] = canonical;
      table[square] = canonical;
      sequenceTable[square] = 0;
      fprintf(stderr, "canonical square: 0x%02x\n", canonical);
    }
  }
  assert(listIndex == MC_COLORED_MARCHING_SQUARES_NUM_CANONICAL_SQUARES);
}

void mcColoredMarchingSquares_printEdgeIntersectionTable(
    mcColoredMarchingSquares_EdgeIntersectionList *table,
    FILE *fp)
{
  fprintf(fp,
      "mcColoredMarchingSquares_EdgeIntersectionList\n"
      "mcColoredMarchingSquares_edgeIntersectionTable[] = {\n");

  /* Iterate over all possible colored square configurations */
  for (int square = 0;
      square < MC_COLORED_MARCHING_SQUARES_NUM_SQUARES;
      ++square)
  {
    fprintf(fp, "  { .edges = { ");
    /* Iterate over all four edges */
    for (int edge = 0; edge < 4; ++edge) {
      fprintf(fp, "%2d, ", table[square].edges[edge]);
    }
    fprintf(fp, "}, },\n");
  }
  fprintf(fp, "};\n");
}

void mcColoredMarchingSquares_printCanonicalSquareTable(
    const int *table,
    FILE *fd)
{
  fprintf(fd,
      "int mcColoredMarchingSquares_canonicalSquareTable[] = {\n");
  /* Iterate over all possible colored square configurations */
  for (int square = 0;
      square < MC_COLORED_MARCHING_SQUARES_NUM_SQUARES;
      ++square)
  {
    if (square % 8 == 0) {
      fprintf(fd, "  ");
    }
    fprintf(fd, "0x%02x, ", table[square]);
    if ((square + 1) % 8 == 0) {
      fprintf(fd, "\n");
    }
  }
  fprintf(fd,
      "};\n");
}

void mcColoredMarchingSquares_printCanonicalSequenceTable(
    const int *table,
    FILE *fd)
{
  fprintf(fd,
      "int mcColoredMarchingSquares_canonicalSequenceTable[] = {\n");
  /* Iterate over all possible colored square configurations */
  for (int square = 0;
      square < MC_COLORED_MARCHING_SQUARES_NUM_SQUARES;
      ++square)
  {
    if (square % 8 == 0) {
      fprintf(fd, "  ");
    }
    fprintf(fd, "0x%04x, ", table[square]);
    if ((square + 1) % 8 == 0) {
      fprintf(fd, "\n");
    }
  }
  fprintf(fd,
      "};\n");
}

void mcColoredMarchingSquares_printCanonicalSquareList(
    const int *list,
    FILE *fd)
{
  fprintf(fd,
      "typedef enum {\n");
  for (int i = 0;
      i < MC_COLORED_MARCHING_SQUARES_NUM_CANONICAL_SQUARES;
      ++i)
  {
    fprintf(fd,
        "  MC_COLORED_MARCHING_SQUARES_CANONICAL_SQUARE_%d = 0x%02x,\n",
        i, list[i]);
  }
  fprintf(fd,
      "} mcColoredMarchingSquares_CanonicalSquare;");
}

void print_usage() {
  fprintf(stderr,
      "Usage:\n"
      "colored_marching_squares_generate_tables [filename]\n\n"
      "Where [filename] is one of the following:\n"
      "    colored_marching_squares_tables.c\n"
      );
}

int main(int argc, char **argv) {
  enum {
    COLORED_MARCHING_SQUARES_TABLES_C,
    COLORED_MARCHING_SQUARES_CANONICAL_H,
  } output;

  mcColoredMarchingSquares_EdgeIntersectionList *edgeIntersectionTable;
  int *canonicalSquareList,
      *canonicalSquareTable,
      *canonicalSequenceTable;;

  /* Parse the arguments to determine which file we are generating */
  if (argc != 2) {
    print_usage();
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "colored_marching_squares_tables.c") == 0) {
    output = COLORED_MARCHING_SQUARES_TABLES_C;
  } else if (strcmp(argv[1], "colored_marching_squares_canonical.h") == 0) {
    output = COLORED_MARCHING_SQUARES_CANONICAL_H;
  } else {
    print_usage();
    return EXIT_FAILURE;
  }

  /* Allocate memory for each table */
  edgeIntersectionTable =
    (mcColoredMarchingSquares_EdgeIntersectionList*)malloc(
        sizeof(mcColoredMarchingSquares_EdgeIntersectionList)
        * MC_COLORED_MARCHING_SQUARES_NUM_SQUARES);
  canonicalSquareList = (int*)malloc(
      sizeof(int) * MC_COLORED_MARCHING_SQUARES_NUM_CANONICAL_SQUARES);
  canonicalSquareTable = (int*)malloc(
      sizeof(int) * MC_COLORED_MARCHING_SQUARES_NUM_SQUARES);
  canonicalSequenceTable = (int*)malloc(
      sizeof(int) * MC_COLORED_MARCHING_SQUARES_NUM_SQUARES);

  /* Compute each table */
  mcColoredMarchingSquares_computeEdgeIntersectionTable(
      edgeIntersectionTable);
  mcColoredMarchingSquares_computeCanonicalSquareTable(
      canonicalSquareList,
      canonicalSquareTable,
      canonicalSequenceTable);

  /* Print each table */
  switch (output) {
    case COLORED_MARCHING_SQUARES_TABLES_C:
      mcColoredMarchingSquares_printEdgeIntersectionTable(
          edgeIntersectionTable, stdout);
      fprintf(stdout, "\n");
      mcColoredMarchingSquares_printCanonicalSquareTable(
          canonicalSquareTable, stdout);
      fprintf(stdout, "\n");
      mcColoredMarchingSquares_printCanonicalSequenceTable(
          canonicalSequenceTable, stdout);
      break;
    case COLORED_MARCHING_SQUARES_CANONICAL_H:
      mcColoredMarchingSquares_printCanonicalSquareList(
          canonicalSquareList, stdout);
      break;
  }
}
