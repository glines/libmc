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
#include <mc/algorithms/simple/common.h>

#include "../common/cube_tables.h"

#define get_byte(num, byte) (((num) & (0xff << (8 * byte))) >> (8 * byte))

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
      mcCube_edgeVertices(edge, vertices);
      if (mcCube_vertexValue(vertices[0], cube)
          != mcCube_vertexValue(vertices[1], cube))
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
  typedef struct IncidentLine {
    unsigned int edges[2];
    unsigned int face;
  } IncidentLine;
  const int MAX_NUM_INCIDENT_LINES = 128;
  int touched[8];
  unsigned int numIncidentLines;
  IncidentLine incidentLines[MAX_NUM_INCIDENT_LINES];

  unsigned int numTriangles;
  unsigned int canonical, rotation;
  mcSimpleTriangle *triangle;

  numIncidentLines = 0;
  memset(touched, 0, sizeof(int) * 8);
  memset(triangleList, -1, sizeof(mcSimpleTriangleList));

  numTriangles = 0;

  /* TODO: Determine this cube's canonical orientation and the corresponding
   * rotation sequences that brings it to that orientation */
  canonical = mcCube_canonicalOrientation(cube);
  rotation = mcCube_canonicalRotation(cube);
  /* TODO: Generate triangles for the canonical orientation */
  switch (canonical) {
    case MC_CUBE_CANONICAL_ORIENTATION_0:
      /* This is a cube entirely inside or outside the isosurface, with no need
       * to generate triangles */
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_1:
      /* This corresponds to a single triangle in one corner */
      triangle = &triangleList->triangles[numTriangles];
      triangle->edges[0] = MC_CUBE_EDGE_BOTTOM_FRONT;
      triangle->edges[1] = MC_CUBE_EDGE_FRONT_RIGHT;
      triangle->edges[2] = MC_CUBE_EDGE_BOTTOM_RIGHT;
      numTriangles += 1;
      fprintf(stderr, "rotation: 0x%08x\n", rotation);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_2:
      /* This is the case where two samples on the same edge are on the other
       * side of the isosurface. This makes a single quad. */
      triangle = &triangleList->triangles[numTriangles];
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_3:
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_4:
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_5:
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_6:
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_7:
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_8:
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_9:
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_10:
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_11:
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_12:
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_13:
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_14:
      break;
  }
  fprintf(stderr, "triangleList before: {\n");
  for (int i = 0; i < MC_SIMPLE_MAX_TRIANGLES; ++i) {
    fprintf(stderr, "  { ");
    for (int j = 0; j < 3; ++j) {
      fprintf(stderr, "%2d, ",
          triangleList->triangles[i].edges[j]);
    }
    fprintf(stderr, "}, \n");
  }
  fprintf(stderr, "}\n");

  /* Rotate the canonical triangles back into our cube's orientation */
  for (int i = 0; i < MC_SIMPLE_MAX_TRIANGLES; ++i) {
    triangle = &triangleList->triangles[i];
    if (triangle->edges[0] == -1)
      break;  /* No more triangles to consider */
    /* Iterate over each triangle edge intersection */
    for (int j = 0; j < 3; ++j) {
      /* Rotate the triangle edge intersection about the y-axis */
      for (int k = 0; k < get_byte(rotation, 2); ++k) {
        triangle->edges[j] = mcCube_rotateEdgeReverseY(triangle->edges[j]);
      }
      /* Rotate the triangle edge intersection about the x-axis */
      for (int k = 0; k < get_byte(rotation, 1); ++k) {
        triangle->edges[j] = mcCube_rotateEdgeReverseX(triangle->edges[j]);
      }
      /* Rotate the triangle edge intersection about the z-axis */
      for (int k = 0; k < get_byte(rotation, 0); ++k) {
        triangle->edges[j] = mcCube_rotateEdgeReverseZ(triangle->edges[j]);
      }
    }
    /* TODO: Consider that cube inversion affects triangle winding order */
  }
}

void printEdgeTable(const mcSimpleEdgeList *edgeTable) {
  fprintf(stdout,
      "const mcSimpleEdgeList mcSimple_edgeTable[] = {\n");
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
      "const mcSimpleTriangleList mcSimple_triangulationTable[] = {\n");
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

    fprintf(stderr, "cube: 0x%02x\n", cube);
    fprintf(stderr, "edgeList: { ");
    for (int i = 0; i < MC_CUBE_NUM_EDGES; ++i) {
      fprintf(stderr, "%2d", edgeTable[cube].edges[i]);
      if (i != MC_CUBE_NUM_EDGES - 1)
        fprintf(stderr, ", ");
    }
    fprintf(stderr, " } \n");
    fprintf(stderr, "triangleList: {\n");
    for (int i = 0; i < MC_SIMPLE_MAX_TRIANGLES; ++i) {
      fprintf(stderr, "  { ");
      for (int j = 0; j < 3; ++j) {
        fprintf(stderr, "%2d, ",
            triangulationTable[cube].triangles[i].edges[j]);
      }
      fprintf(stderr, "}, \n");
    }
    fprintf(stderr, "}\n");

#ifndef NDEBUG
    /* Ensure that the edge and triangulation tables agree */
    for (int i = 0; i < MC_SIMPLE_MAX_TRIANGLES; ++i) {
      mcSimpleTriangle *triangle = &triangulationTable[cube].triangles[i];
      if (triangle->edges[0] == -1)
        break;  /* No more triangles to consider */
      for (int j = 0; j < 3; ++j) {
        int found;
        unsigned int edge = triangle->edges[j];
        /* Look for this edge in the edge list */
        found = 0;
        for (int k = 0; k < MC_CUBE_NUM_EDGES; ++k) {
          if (edgeTable[cube].edges[k] == edge) {
            found = 1;
            break;
          }
        }
        assert(found);
      }
    }
#endif
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
