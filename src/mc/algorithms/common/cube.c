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

#define MC_CUBE_HAVE_TABLES
#include <mc/algorithms/common/cube.h>

#include "cube_tables.h"

void mcCube_edgeVertices(unsigned int edge, unsigned int *vertices) {
  typedef struct VertexPair {
    int vertices[2];
  } VertexPair;
  static const VertexPair table[] = {
    { .vertices = { 0, 1 } },  /* Edge 0 */
    { .vertices = { 1, 2 } },  /* Edge 1 */
    { .vertices = { 2, 3 } },  /* Edge 2 */
    { .vertices = { 0, 3 } },  /* Edge 3 */
    { .vertices = { 4, 5 } },  /* Edge 4 */
    { .vertices = { 5, 6 } },  /* Edge 5 */
    { .vertices = { 6, 7 } },  /* Edge 6 */
    { .vertices = { 4, 7 } },  /* Edge 7 */
    { .vertices = { 0, 4 } },  /* Edge 8 */
    { .vertices = { 1, 5 } },  /* Edge 9 */
    { .vertices = { 3, 7 } },  /* Edge 10 */
    { .vertices = { 2, 6 } },  /* Edge 11 */
  };
  assert(edge < 12);
  vertices[0] = table[edge].vertices[0];
  vertices[1] = table[edge].vertices[1];
}

void mcCube_edgeFaces(unsigned int edge, unsigned int *faces) {
  typedef struct FacePair {
    int faces[2];
  } FacePair;
  static const FacePair table[] = {
    { .faces = { 0, 4 } },  /* Edge 0 */
    { .faces = { 0, 1 } },  /* Edge 1 */
    { .faces = { 0, 2 } },  /* Edge 2 */
    { .faces = { 0, 3 } },  /* Edge 3 */
    { .faces = { 4, 5 } },  /* Edge 4 */
    { .faces = { 1, 5 } },  /* Edge 5 */
    { .faces = { 2, 5 } },  /* Edge 6 */
    { .faces = { 3, 5 } },  /* Edge 7 */
    { .faces = { 3, 4 } },  /* Edge 8 */
    { .faces = { 1, 4 } },  /* Edge 9 */
    { .faces = { 2, 3 } },  /* Edge 10 */
    { .faces = { 1, 2 } },  /* Edge 11 */
  };
  assert (edge < 12);
  faces[0] = table[edge].faces[0];
  faces[1] = table[edge].faces[1];
}

int mcCube_verticesToEdge(unsigned int a, unsigned int b) {
  assert(a >= 0);
  assert(a < 8);
  assert(b >= 0);
  assert(b < 8);
  switch (a) {
    case 0:
      switch (b) {
        case 1:
          return 0;
        case 3:
          return 4;
        case 4:
          return 8;
      }
      break;
    case 1:
      switch (b) {
        case 0:
          return 0;
        case 2:
          return 1;
        case 5:
          return 9;
      }
      break;
    case 2:
      switch (b) {
        case 1:
          return 1;
        case 3:
          return 2;
        case 6:
          return 11;
      }
      break;
    case 3:
      switch (b) {
        case 0:
          return 3;
        case 2:
          return 2;
        case 7:
          return 10;
      }
      break;
    case 4:
      switch (b) {
        case 0:
          return 8;
        case 5:
          return 4;
        case 7:
          return 7;
      }
      break;
    case 5:
      switch (b) {
        case 1:
          return 9;
        case 4:
          return 4;
        case 6:
          return 5;
      }
      break;
    case 6:
      switch (b) {
        case 2:
          return 11;
        case 5:
          return 5;
        case 7:
          return 6;
      }
      break;
    case 7:
      switch (b) {
        case 3:
          return 10;
        case 4:
          return 7;
        case 6:
          return 6;
      }
      break;
  }
  return -1;
}

void mcCube_vertexEdges(unsigned int vertex, int *edges) {
  typedef struct EdgeTriple {
    int edges[3];
  } EdgeTriple;
  static const EdgeTriple table[] = {
    { .edges = { 0, 3, 8 } },
    { .edges = { 0, 1, 9 } },
    { .edges = { 1, 2, 11 } },
    { .edges = { 2, 3, 10 } },
    { .edges = { 4, 7, 8 } },
    { .edges = { 4, 5, 9 } },
    { .edges = { 5, 6, 11 } },
    { .edges = { 6, 7, 10 } },
  };
  assert(vertex < 8);
  edges[0] = table[vertex].edges[0];
  edges[1] = table[vertex].edges[1];
  edges[2] = table[vertex].edges[2];
}

void mcCube_adjacentVertices(unsigned int vertex, unsigned int *adjacent) {
  typedef struct VertexTriple {
    int vertices[3];
  } VertexTriple;
  static const VertexTriple table[] = {
    { .vertices = { 1, 3, 4 } },  /* Vertex 0 */
    { .vertices = { 0, 2, 5 } },  /* Vertex 1 */
    { .vertices = { 1, 3, 6 } },  /* Vertex 2 */
    { .vertices = { 0, 2, 7 } },  /* Vertex 3 */
    { .vertices = { 0, 5, 7 } },  /* Vertex 4 */
    { .vertices = { 1, 4, 6 } },  /* Vertex 5 */
    { .vertices = { 2, 5, 7 } },  /* Vertex 6 */
    { .vertices = { 3, 4, 6 } },  /* Vertex 7 */
  };
  assert(vertex < 8);
  adjacent[0] = table[vertex].vertices[0];
  adjacent[1] = table[vertex].vertices[1];
  adjacent[2] = table[vertex].vertices[2];
}

int mcCube_vertexValue(unsigned int vertex, unsigned int cube) {
  return (cube & (1 << vertex)) >> vertex;
}

unsigned int mcCube_vertexIndex(unsigned int x, unsigned int y, unsigned int z) {
  assert((x & ~1) == 0);
  assert((y & ~1) == 0);
  assert((z & ~1) == 0);
  int i = x | y << 1 | z << 2;
  static const unsigned int table[] = {
    0,  // x = 0, y = 0, z = 0
    1,  // x = 1, y = 0, z = 0
    3,  // x = 0, y = 1, z = 0
    2,  // x = 1, y = 1, z = 0
    4,  // x = 0, y = 0, z = 1
    5,  // x = 1, y = 0, z = 1
    7,  // x = 0, y = 1, z = 1
    6   // x = 1, y = 1, z = 1
  };
  return table[i];
}

void mcCube_vertexClosure(unsigned int vertex, unsigned int cube,
    unsigned int *closure, unsigned int *closureSize)
{
  unsigned int notVisited[8];
  unsigned int numNotVisited;
  unsigned int visited[8];
  unsigned int numVisited;
  unsigned int adjacent[3];
  int skip;
  int i, j;
  int vertexValue = mcCube_vertexValue(vertex, cube);

  notVisited[0] = vertex;
  numNotVisited = 1;

  numVisited = 0;

  *closureSize = 0;

  /* Visit all vertices we have not yet visited */
  while (numNotVisited > 0) {
    /* Pop a vertex not yet visited from the stack */
    numNotVisited -= 1;
    /* Add this vertex to the list of visited vertices */
    visited[numVisited++] = notVisited[numNotVisited];
    /* Add this vertex to the closure */
    closure[(*closureSize)++] = notVisited[numNotVisited];
    /* Iterate over all adjacent vertices */
    mcCube_adjacentVertices(notVisited[numNotVisited], adjacent);
    for (i = 0; i < 3; ++i) {
      /* Check if this vertex has already been visited */
      skip = 0;
      for (j = 0; j < numVisited; ++j) {
        if (adjacent[i] == visited[j]) {
          skip = 1;
          break;
        }
      }
      if (skip)
        continue;
      /* Add alike vertices to the list of not yet visited vertices */
      if (mcCube_vertexValue(adjacent[i], cube) == vertexValue)
        notVisited[numNotVisited++] = adjacent[i];
    }
  }
}

void mcCube_boundryEdges(unsigned int vertex, unsigned int cube,
    unsigned int *edges, unsigned int *numEdges)
{
  unsigned int notVisited[8];
  unsigned int numNotVisited;
  unsigned int current;
  unsigned int visited[8];
  unsigned int numVisited;
  unsigned int adjacent[3];
  int skip;
  int i, j;
  int vertexValue = mcCube_vertexValue(vertex, cube);

  notVisited[0] = vertex;
  numNotVisited = 1;

  numVisited = 0;

  *numEdges = 0;

  /* Visit all vertices we have not yet visited */
  while (numNotVisited > 0) {
    /* Pop a vertex not yet visited from the stack */
    numNotVisited -= 1;
    current = notVisited[numNotVisited];
    /* Add this vertex to the list of visited vertices */
    visited[numVisited++] = current;
    /* Iterate over all adjacent vertices */
    mcCube_adjacentVertices(current, adjacent);
    for (i = 0; i < 3; ++i) {
      /* Check if this vertex has already been visited */
      skip = 0;
      for (j = 0; j < numVisited; ++j) {
        if (adjacent[i] == visited[j]) {
          skip = 1;
          break;
        }
      }
      if (skip)
        continue;
      if (mcCube_vertexValue(adjacent[i], cube) == vertexValue) {
        /* Add alike vertices to the list of not yet visited vertices */
        notVisited[numNotVisited++] = adjacent[i];
      } else {
        /* Add boundry edges to the list of edges */
        edges[(*numEdges)++] = mcCube_verticesToEdge(current, adjacent[i]);
      }
    }
  }
}

void mcCube_vertexRelativePosition(unsigned int vertex, unsigned int *pos) {
  typedef struct Position {
    unsigned int pos[3];
  } Position;
  Position table[8] = {
    { .pos = { 0, 0, 0 } },  /* vertex 0 */
    { .pos = { 1, 0, 0 } },  /* vertex 1 */
    { .pos = { 1, 1, 0 } },  /* vertex 2 */
    { .pos = { 0, 1, 0 } },  /* vertex 3 */
    { .pos = { 0, 0, 1 } },  /* vertex 4 */
    { .pos = { 1, 0, 1 } },  /* vertex 5 */
    { .pos = { 1, 1, 1 } },  /* vertex 6 */
    { .pos = { 0, 1, 1 } },  /* vertex 7 */
  };
  assert(vertex < 8);
  pos[0] = table[vertex].pos[0];
  pos[1] = table[vertex].pos[1];
  pos[2] = table[vertex].pos[2];
}

unsigned int mcCube_cubeConfigurationFromSamples(float *samples) {
  unsigned int cube = 0;
  for (unsigned int vertex = 0; vertex < 8; ++vertex) {
    /* Add the bit this vertex contributes to the cube */
    cube |= (samples[vertex] >= 0.0f ? 1 : 0) << vertex;
  }
  assert(cube <= 0xff);
  return cube;
}

unsigned int mcCube_canonicalOrientation(unsigned int cube) {
  assert(cube <= 0xff);
  return mcCubeCanonicalOrientationTable[cube];
}

unsigned int mcCube_canonicalRotation(unsigned int cube) {
  assert(cube <= 0xff);
  return mcCubeCanonicalRotationTable[cube];
}

unsigned int mcCube_rotateEdgeX(unsigned int edge) {
  assert(edge < MC_CUBE_NUM_EDGES);
  return mcCubeEdgeRotationTableX[edge];
}

unsigned int mcCube_rotateEdgeY(unsigned int edge) {
  assert(edge < MC_CUBE_NUM_EDGES);
  return mcCubeEdgeRotationTableY[edge];
}

unsigned int mcCube_rotateEdgeZ(unsigned int edge) {
  assert(edge < MC_CUBE_NUM_EDGES);
  return mcCubeEdgeRotationTableZ[edge];
}

unsigned int mcCube_rotateEdgeReverseX(unsigned int edge) {
  assert(edge < MC_CUBE_NUM_EDGES);
  return mcCubeEdgeReverseRotationTableX[edge];
}

unsigned int mcCube_rotateEdgeReverseY(unsigned int edge) {
  assert(edge < MC_CUBE_NUM_EDGES);
  return mcCubeEdgeReverseRotationTableY[edge];
}

unsigned int mcCube_rotateEdgeReverseZ(unsigned int edge) {
  assert(edge < MC_CUBE_NUM_EDGES);
  return mcCubeEdgeReverseRotationTableZ[edge];
}
