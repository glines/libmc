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

#include "common.h"

/*
 * This program generates the edge and triangularization tables needed for
 * implementing a performant marching cubes algorithm. While these tables are
 * available pre-generated on the internet, generating these tables ourselves
 * serves as a stepping stone towards generating larger tables used by more
 * sophisticated variants of the marching cubes algorithm.
 *
 * The first table generated is the edge table, whose purpose is to determine
 * which edges are intersected for a given voxel configuration. The marching
 * cubes algorithm uses this information to interpolate between samples on the
 * relevent edges. This table is relatively easy to generate, since any edge
 * whose samples fall on different sides of the isosurface must be intersected
 * by the isosurface.
 *
 * The second table generated is the triangularization table. This table is
 * used to quickly find a triangle representation for the voxel that can be
 * used in the resulting surface mesh. This table is much more complicated to
 * generate, but it can build upon the edge table since triangle vertices must
 * lie on intersected edges.
 */

void computeEdgeList(
    unsigned int cube,
    mcSimpleEdgeList *edgeList)
{
    unsigned int vertices[2];
    unsigned int listIndex = 0;
    /* Iterate through all edges */
    for (unsigned int edge = 0; edge < 12; ++edge) {
      /* Determine the two vertex values */
      mcSimpleEdgeVertices(edge, vertices);
      if (mcSimpleVertexValue(vertices[0], cube)
          != mcSimpleVertexValue(vertices[1], cube))
      {
        /* If the vertex values disagree, we have an edge intersection */
        /* Add this edge to the edge list */
        edgeList->edges[listIndex++] = edge;
      }
    }
}

void computeTriangleList(
    unsigned int cube,
    mcSimpleTriangleList *triangleList)
{
  int touched[8];
  unsigned int adjacent[3];
  unsigned int closure[8];
  unsigned int closureSize;
  int i;

  i = 0;

  memset(touched, 0, sizeof(int) * 8);

  /* TODO: Determine which side of the isosurface most of our vertices lie
   *       (the Hamming weight of cube)
   */

  /* TODO: Iterate through untouched vertices of interest */
  for (int vertex = 0; vertex < 8; ++vertex) {
    if (touched[vertex])
      continue;
    if (!mcSimpleVertexValue(vertex, cube))
      continue;
    /* TODO: Traverse the graph of adjacent vertices of interest */
    mcSimpleAdjacentVertices(vertex, adjacent);
    /* TODO: Determine the extent of the intersected edges for this graph */
    mcSimpleVertexClosure(vertex, cube, closure, &closureSize);
    /* TODO: Determine the shape */
    switch (closureSize) {
      case 1:
        /* Generate a single triangle using the edges */
        mcSimpleVertexEdges(vertex, triangleList[i].triangles[0].edges);
        break;
      case 2:
        /* TODO: Generate a quad */
        break;
    }
  }
}

void printEdgeTable(const mcSimpleEdgeList *edgeTable) {
  fprintf(stdout,
      "const mcSimpleEdgeList mcSimpleEdgeTable[] = {\n");
  for (unsigned int cube = 0; cube <= 0xFF; ++cube) {
    fprintf(stdout,
        "  { .edges = { %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d } },\n",
        edgeTable[cube].edges[0],
        edgeTable[cube].edges[1],
        edgeTable[cube].edges[2],
        edgeTable[cube].edges[3],
        edgeTable[cube].edges[4],
        edgeTable[cube].edges[5],
        edgeTable[cube].edges[6],
        edgeTable[cube].edges[7],
        edgeTable[cube].edges[8],
        edgeTable[cube].edges[9],
        edgeTable[cube].edges[10],
        edgeTable[cube].edges[11]
        );
  }
  fprintf(stdout,
      "};\n");
}

void printTriangulationTable(
    const mcSimpleTriangleList *triangulationTable)
{
  fprintf(stdout,
      "const mcSimpleTriangleList mcSimpleTriangulationTable[] = {\n");
  for (unsigned int cube = 0; cube <= 0xFF; ++cube) {
    fprintf(stdout,
        "  { .triangles = \n"
        "    {\n"
        "      { .edges = { %d, %d, %d } },\n"
        "      { .edges = { %d, %d, %d } },\n"
        "      { .edges = { %d, %d, %d } },\n"
        "      { .edges = { %d, %d, %d } },\n"
        "    },\n"
        "  },\n",
        triangulationTable[cube].triangles[0].edges[0],
        triangulationTable[cube].triangles[0].edges[1],
        triangulationTable[cube].triangles[0].edges[2],
        triangulationTable[cube].triangles[1].edges[0],
        triangulationTable[cube].triangles[1].edges[1],
        triangulationTable[cube].triangles[1].edges[2],
        triangulationTable[cube].triangles[2].edges[0],
        triangulationTable[cube].triangles[2].edges[1],
        triangulationTable[cube].triangles[2].edges[2],
        triangulationTable[cube].triangles[3].edges[0],
        triangulationTable[cube].triangles[3].edges[1],
        triangulationTable[cube].triangles[3].edges[2]
        );
  }
  fprintf(stdout,
      "};\n");
}

int main(int argc, char **argv) {
  /* TODO: Parse the arguments */

  /* Allocate memory for the edge table */
  mcSimpleEdgeList *edgeTable =
    (mcSimpleEdgeList*)malloc(sizeof(mcSimpleEdgeList) * 256);
  memset(edgeTable, -1, sizeof(mcSimpleEdgeList) * 256);
  /* Allocate memory for the triangulization table */
  mcSimpleTriangleList *triangulationTable =
    (mcSimpleTriangleList*)malloc(sizeof(mcSimpleTriangleList) * 256);
  memset(triangulationTable, -1, sizeof(mcSimpleTriangleList) * 256);

  /* Iterate through all voxel cube configurations */
  for (unsigned int cube = 0; cube <= 0xFF; ++cube) {

    /* Compute the edge list for this configuration */
    computeEdgeList(cube, &edgeTable[cube]); 

    /* Compute the triangulation list for this configuration */
    computeTriangleList(cube, &triangulationTable[cube]);
  }

  /* Print the necessary headers */
  fprintf(stdout, "#include \"common.h\"\n\n");

  /* Print the edge table */
  printEdgeTable(edgeTable);

  fprintf(stdout, "\n");

  /* Print the triangulation table */
  printTriangulationTable(triangulationTable);

  free(edgeTable);
  free(triangulationTable);
}
