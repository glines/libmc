#include <assert.h>

void mcEdgeVertices(unsigned int edge, unsigned int *vertices) {
  typedef struct VertexPair {
    int vertex[2];
  } VertexPair;
  static VertexPair vertexTable[] = {
    {  /* Edge 0 */
      .vertex[0] = 0,
      .vertex[1] = 1,
    },
    {  /* Edge 1 */
      .vertex[0] = 1,
      .vertex[1] = 2,
    },
  };
  /* TODO: Replace this with a static table? */
  switch (edge) {
    case 0:
      vertices[0] = 0;
      vertices[1] = 1;
      break;
    case 1:
      vertices[0] = 1;
      vertices[1] = 2;
      break;
    case 2:
      vertices[0] = 2;
      vertices[1] = 3;
      break;
    case 3:
      vertices[0] = 0;
      vertices[1] = 3;
      break;
    case 4:
      vertices[0] = 4;
      vertices[1] = 5;
      break;
    case 5:
      vertices[0] = 5;
      vertices[1] = 6;
      break;
    case 6:
      vertices[0] = 6;
      vertices[1] = 7;
      break;
    case 7:
      vertices[0] = 4;
      vertices[1] = 7;
      break;
    case 8:
      vertices[0] = 0;
      vertices[1] = 4;
      break;
    case 9:
      vertices[0] = 1;
      vertices[1] = 5;
      break;
    case 10:
      vertices[0] = 3;
      vertices[1] = 7;
      break;
    case 11:
      vertices[0] = 2;
      vertices[1] = 6;
      break;
  }
}

void mcVertexEdges(unsigned int vertex, unsigned int *edges) {
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

void mcAdjacentVertices(unsigned int vertex, unsigned int *adjacent) {
  typedef struct VertexTriple {
    int vertices[3];
  } VertexTriple;
  static const VertexTriple table[] = {
    { .vertices = { 1, 3, 4 } },
    { .vertices = { 0, 2, 5 } },
    { .vertices = { 1, 3, 6 } },
    { .vertices = { 0, 2, 7 } },
    { .vertices = { 0, 5, 7 } },
    { .vertices = { 1, 4, 6 } },
    { .vertices = { 2, 5, 7 } },
    { .vertices = { 3, 4, 6 } },
  };
  assert(vertex < 8);
  adjacent[0] = table[vertex].vertices[0];
  adjacent[1] = table[vertex].vertices[1];
  adjacent[2] = table[vertex].vertices[2];
}

int mcVertexValue(unsigned int vertex, unsigned int cube) {
  return (cube & (1 << vertex)) >> vertex;
}

unsigned int mcVertexIndex(unsigned int x, unsigned int y, unsigned int z) {
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

void mcVertexClosure(unsigned int vertex, unsigned int cube,
    unsigned int *closure, unsigned int *closureSize)
{
  unsigned int notVisited[8];
  unsigned int numNotVisited;
  unsigned int visited[8];
  unsigned int numVisited;
  unsigned int adjacent[3];
  int skip;
  int i, j;
  int vertexValue = mcVertexValue(vertex, cube);

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
    mcAdjacentVertices(notVisited[numNotVisited], adjacent);
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
      if (mcVertexValue(adjacent[i], cube) == vertexValue)
        notVisited[numNotVisited++] = adjacent[i];
    }
  }
}
