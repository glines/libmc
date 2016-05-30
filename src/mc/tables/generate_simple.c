#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "../simple.h"

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

typedef struct EdgeList {
  unsigned int edges[12];
} EdgeList;

typedef struct Triangle {
  unsigned int edges[3];
} Triangle;

typedef struct TriangleList {
  Triangle triangles[4];
} TriangleList;

void computeEdgeList(
    unsigned int cube,
    EdgeList *edgeList)
{
    unsigned int vertices[2];
    unsigned int listIndex = 0;
    /* Iterate through all edges */
    for (unsigned int edge = 0; edge < 12; ++edge) {
      /* Determine the two vertex values */
      mcEdgeVertices(edge, vertices);
      if (mcVertexValue(vertices[0], cube)
          != mcVertexValue(vertices[1], cube))
      {
        /* If the vertex values disagree, we have an edge intersection */
        /* Add this edge to the edge list */
        edgeList->edges[listIndex++] = edge;
      }
    }
}

void computeTriangleList(
    unsigned int cube,
    TriangleList *triangleList)
{
  int touched[8];
  int adjacent[3];
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
    if (!mcVertexValue(vertex, cube))
      continue;
    /* TODO: Traverse the graph of adjacent vertices of interest */
    mcAdjacentVertices(vertex, adjacent);
    /* TODO: Determine the extent of the intersected edges for this graph */
    mcVertexClosure(vertex, cube, closure, &closureSize);
    /* TODO: Determine the shape */
    switch (closureSize) {
      case 1:
        /* Generate a single triangle using the edges */
        mcVertexEdges(vertex, triangleList[i].triangles[0].edges);
        break;
      case 2:
        /* TODO: Generate a quad */
        break;
    }
  }
}

void printEdgeTable(const EdgeList *edgeTable) {
  fprintf(stdout,
      "const EdgeList mcEdgeTable[] = {\n");
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
        edgeTable[cube].edges[11],
        edgeTable[cube].edges[12]
        );
  }
  fprintf(stdout,
      "};\n");
}

void printTriangulationTable(const TriangleList *triangulationTable) {
  fprintf(stdout,
      "const TriangleList mcTriangulationTable[] = {\n");
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
  EdgeList *edgeTable = (EdgeList*)malloc(sizeof(EdgeList) * 256);
  memset(edgeTable, -1, sizeof(EdgeList) * 256);
  /* Allocate memory for the triangulization table */
  TriangleList *triangulationTable =
    (TriangleList*)malloc(sizeof(TriangleList) * 256);
  memset(triangulationTable, -1, sizeof(TriangleList) * 256);

  /* Iterate through all voxel cube configurations */
  for (unsigned int cube = 0; cube <= 0xFF; ++cube) {

    /* Compute the edge list for this configuration */
    computeEdgeList(cube, &edgeTable[cube]); 

    /* Compute the triangulation list for this configuration */
    computeTriangleList(cube, &triangulationTable[cube]);
  }

  /* Print the edge table */
  printEdgeTable(edgeTable);

  /* Print the triangulation table */
  printTriangulationTable(triangulationTable);

  free(edgeTable);
  free(triangulationTable);
}
