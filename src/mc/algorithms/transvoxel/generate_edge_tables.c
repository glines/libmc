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

void mcTransvoxel_computeTransitionCellEdgeSamplesTable(
    int *table)
{
  static const int edgeSamplesTable[] = {
    0, 2,  /* Edge 0 */
    2, 8,  /* Edge 1 */
    6, 8,  /* Edge 2 */
    0, 6,  /* Edge 3 */
    0, 1,  /* Edge 4 */
    1, 2,  /* Edge 5 */
    3, 4,  /* Edge 6 */
    4, 5,  /* Edge 7 */
    6, 7,  /* Edge 8 */
    7, 8,  /* Edge 9 */
    0, 3,  /* Edge 10 */
    1, 4,  /* Edge 11 */
    2, 5,  /* Edge 12 */
    3, 6,  /* Edge 13 */
    4, 7,  /* Edge 14 */
    5, 8,  /* Edge 15 */
  };
  memcpy(table, edgeSamplesTable, sizeof(edgeSamplesTable));
}

void mcTransvoxel_computeTransitionCellEdgeIntersectionTable(
    const int *edgeSamplesTable,
    mcTransvoxel_TransitionCellEdgeIntersectionList *table)
{
  /* Iterate through all transition cell configurations */
  for (int cell = 0; cell < MC_TRANSVOXEL_NUM_TRANSITION_CELLS; ++cell) {
    mcTransvoxel_TransitionCellEdgeIntersectionList *list = &table[cell];
    memset(list->edges, -1, sizeof(list->edges));
    int listIndex = 0;
    /* Iterate through the transition cell edges */
    for (int edge = 0; edge < MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES; ++edge) {
      const int *sampleIndices = &edgeSamplesTable[edge * 2];
#define SAMPLE_VALUE(cell, index) (((cell) & (1 << (index))) >> (index))
      if (SAMPLE_VALUE(cell, sampleIndices[0])
          != SAMPLE_VALUE(cell, sampleIndices[1]))
      {
        /* If the sample values disagree, we have an edge intersection */
        /* Add this edge intersection to the list */
        list->edges[listIndex++] = edge;
      }
    }
  }
}

void mcTransvoxel_printTransitionCellEdgeSamplesTable(int *table, FILE *fd) {
  fprintf(fd, "const int mcTransvoxel_transitionCellEdgeSamplesTable[] = {\n");
  /* TODO: Loop through all of the transition cell edges */
  for (
      int edge = 0;
      edge < MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES;
      ++edge)
  {
    fprintf(fd, "  %d, %d,  /* Edge %d */\n",
        table[edge * 2],
        table[edge * 2 + 1],
        edge);
  }
  fprintf(fd, "};\n");
}

void mcTransvoxel_printTransitionCellEdgeIntersectionTable(
    mcTransvoxel_TransitionCellEdgeIntersectionList *table, FILE *fd)
{
  fprintf(fd,
      "const mcTransvoxel_TransitionCellEdgeIntersectionList\n"
      "mcTransvoxel_transitionCellEdgeIntersectionTable[] = {\n");
  /* Loop through all of the transition cell configurations */
  for (
      int cell = 0;
      cell < MC_TRANSVOXEL_NUM_TRANSITION_CELLS;
      ++cell)
  {
    mcTransvoxel_TransitionCellEdgeIntersectionList *list = &table[cell];
    fprintf(fd,
        "  { .edges = {  ");
    /* Loop through all of the edges we might have in our edge intersection
     * list */
    for (
        int edge = 0;
        edge < MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES;
        ++edge)
    {
      fprintf(fd, "%2d, ",
          list->edges[edge]);
    }
    fprintf(fd, "}, },\n");
  }
  fprintf(fd, "};\n");
}

void print_usage() {
  fprintf(stderr,
      "Usage:\n"
      "transvoxel_generate_edge_tables [filename]\n\n"
      "Where [filename] is one of the following:\n"
      "    transvoxel_edge_tables.c\n\n"
      );
}

int main(int argc, char **argv) {
  enum {
    TRANSVOXEL_EDGE_TABLES_C,
  } output;

  int *transitionCellEdgeSamplesTable;
  mcTransvoxel_TransitionCellEdgeIntersectionList
    *transitionCellEdgeIntersectionTable;

  /* Parse the arguments to determine which file we are generating */
  if (argc != 2) {
    print_usage();
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "transvoxel_edge_tables.c") == 0) {
    output = TRANSVOXEL_EDGE_TABLES_C;
  } else {
    print_usage();
    return EXIT_FAILURE;
  }

  /* Allocate memory for each table */
  transitionCellEdgeSamplesTable = malloc(
      sizeof(int) * 2 * MC_TRANSVOXEL_NUM_TRANSITION_CELL_EDGES);
  transitionCellEdgeIntersectionTable = malloc(
      sizeof(mcTransvoxel_TransitionCellEdgeIntersectionList)
      * MC_TRANSVOXEL_NUM_TRANSITION_CELLS);

  /* Compute the tables */
  mcTransvoxel_computeTransitionCellEdgeSamplesTable(
      transitionCellEdgeSamplesTable);
  mcTransvoxel_computeTransitionCellEdgeIntersectionTable(
      transitionCellEdgeSamplesTable,
      transitionCellEdgeIntersectionTable);

  /* Print the tables */
  switch (output) {
    case TRANSVOXEL_EDGE_TABLES_C:
      mcTransvoxel_printTransitionCellEdgeSamplesTable(
          transitionCellEdgeSamplesTable, stdout);
      fprintf(stdout, "\n");
      mcTransvoxel_printTransitionCellEdgeIntersectionTable(
          transitionCellEdgeIntersectionTable, stdout);
      break;
  }

  /* Free table memory */
  free(transitionCellEdgeIntersectionTable);
  free(transitionCellEdgeSamplesTable);
}
