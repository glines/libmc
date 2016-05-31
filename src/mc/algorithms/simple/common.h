const unsigned int MC_SIMPLE_MAX_EDGES = 12;

typedef struct mcSimpleEdgeList {
  unsigned int edges[12];
} mcSimpleEdgeList;

typedef struct mcSimpleTriangle {
  unsigned int edges[3];
} mcSimpleTriangle;

typedef struct mcSimpleTriangleList {
  mcSimpleTriangle triangles[4];
} mcSimpleTriangleList;

/**
 * This routine takes the index of an edge and returns the two vertex indices
 * on that edge in vertices.
 *
 * These indices are based on the numbering scheme described by the original
 * marching cubes paper by Lorensen.
 */
void mcSimpleEdgeVertices(unsigned int edge, unsigned int *vertices);

/**
 * This routine determines the edges connected to a given vertex.
 */
void mcSimpleVertexEdges(unsigned int vertex, unsigned int *edges);

/**
 * This routine determines the vertices adjacent to the given vertex and
 * returns the adjacent vertex indices in adjacent.
 */
void mcSimpleAdjacentVertices(unsigned int vertex, unsigned int *adjacent);

/**
 * This routine determines the value of a vertex (either above or below the
 * isosurface) for the given numbered cube configuration.
 *
 * The returned value is 0 for less than the isosurface value, and 1 for
 * greater than or equal to the isosurface value.
 */
int mcSimpleVertexValue(unsigned int vertex, unsigned int cube);

/**
 * This routine takes a vertex position on the unit cube with vertex 0
 * positioned at the origin and returns the vertex index.
 */
unsigned int mcSimpleVertexIndex(
    unsigned int x, unsigned int y, unsigned int z);

/**
 * This routine traverses the edges of alike vertices to compute the closure of
 * a given vertex.
 */
void mcSimpleVertexClosure(unsigned int vertex, unsigned int cube,
    unsigned int *closure, unsigned int *closureSize);

/**
 * This routine determines the XYZ-position of the given vertex relative to the
 * origin of the cube.
 */
void mcSimpleVertexRelativePosition(unsigned int vertex, unsigned int *pos);
