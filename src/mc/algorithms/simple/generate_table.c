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

#include <mc/algorithms/simple/common.h>

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
    const mcSimpleEdgeList *edgeList,
    mcSimpleTriangleList *triangleList)
{
  int touched[8];
  unsigned int adjacent[3];
  unsigned int closure[8];
  unsigned int closureSize;
  unsigned int numIncidentLines;
  int i;

  i = 0;

  numIncidentLines = 0;

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
    /* FIXME: I want an edge closure, not a vertex closure */
    mcSimpleVertexClosure(vertex, cube, closure, &closureSize);
    /* TODO: Determine the shape */
    /* TODO: Our triangulation strategy will proceed as follows: 
     * We are given a list of edge intersections from our edge intersection
     * table.
     *
     * From these intersections, we build a list of lines from edge to
     * edge that are incident with the surface of the cube.
     *
     * From this list of lines, we remove two lines whose incident cube faces
     * are adjacent. We make a triangle with these two lines. If the third line
     * in the triangle is incident with the surface of the cube, we remove it
     * from our list of lines. If it is not incident with the surface of the
     * cube, we add it to a separate list of unused lines.
     *
     * We iterate the previous step until we can find no two lines whose
     * incident cube faces are adjacent.
     *
     * Now we just need to fill the gaps caused by the unused lines. At this
     * point, the unused lines might be coincident (i.e. we have no gap to
     * fill), or they might form a triangle or a quad.
     *
     * TODO: What do do with the separate list of lines?
     */
    for (int i = 0; i < MC_SIMPLE_MAX_EDGES && edgeList->edges[i] != -1; ++i) {
      for (int j = i + 1; j < MC_SIMPLE_MAX_EDGES && edgeList->edges[j] != -1; ++j) {
        /* TODO: Look for a cube face common to both edges */
        /* TODO: Move this into mcSimpleEdgesSharedFace() routine */
        unsigned int faces_i[2], faces_j[2];
        int shared = -1;
        mcSimpleEdgeFaces(edgeList->edges[i], faces_i);
        mcSimpleEdgeFaces(edgeList->edges[j], faces_j);
        for (int k = 0; k < 2; ++k) {
          for (int l = 0; l < 2; ++l) {
            if (faces_i[k] == faces_j[l]) {
              shared = faces_i[k];
              break;
            }
          }
          if (shared != -1)
            break;
        }
        if (shared != -1) {
          /* TODO: Add this edge pair to the list of incident lines */
          numIncidentLines += 1;
        }
      }
    }
    fprintf(stderr, "cube: 0x%02x, numIncidentLines: %d\n", cube, numIncidentLines);
    numIncidentLines = 0;
    switch (closureSize) {
      case 1:
        /* Generate a single triangle using the edges */
        mcSimpleVertexEdges(vertex, triangleList[i].triangles[0].edges);
        break;
/*      case 2: */
        /* TODO: Generate a quad */
        /* TODO: Determine which edge these vertices share */
        /* TODO: Generate a quad intersecting the edges that these vertices do
         * not share */
    }
  }
}

void printEdgeTable(const mcSimpleEdgeList *edgeTable) {
  fprintf(stdout,
      "const mcSimpleEdgeList mcSimpleEdgeTable[] = {\n");
  for (unsigned int cube = 0; cube <= 0xFF; ++cube) {
    fprintf(stdout,
        "  { .edges = { %2d, %2d, %2d, %2d, %2d, %2d, %2d, %2d, %2d, %2d, %2d, %2d } },  /* 0x%02x */\n",
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
        cube
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
    computeTriangleList(cube, &edgeTable[cube], &triangulationTable[cube]);
  }

  /* Print the necessary headers */
  fprintf(stdout, "#include <mc/algorithms/simple/common.h>\n\n");

  /* Print the edge table */
  printEdgeTable(edgeTable);

  fprintf(stdout, "\n");

  /* Print the triangulation table */
  printTriangulationTable(triangulationTable);

  free(edgeTable);
  free(triangulationTable);
}
