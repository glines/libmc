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

extern const unsigned int MC_SIMPLE_MAX_EDGES;

typedef struct mcSimpleEdgeList {
  int edges[12];
} mcSimpleEdgeList;

typedef struct mcSimpleTriangle {
  int edges[3];
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
 * This routine determines the two cube faces that the given edge lies on and
 * returns the indices of those faces in faces.
 */
void mcSimpleEdgeFaces(unsigned int edge, unsigned int *faces);

/**
 * This routine determines the edge that lies between the two given vertices
 * and returns that edge number. If the line between a and b is not incident to
 * the cube, or if a and b are the same vertex, then -1 is returned instead.
 */
int mcSimpleVerticesToEdge(unsigned int a, unsigned int b);

/**
 * This routine determines the edges connected to a given vertex.
 */
void mcSimpleVertexEdges(unsigned int vertex, int *edges);

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
 * This starts at a vertex and traverses adjacent edges of alike vertices to
 * find the boundry edges on the cube.
 *
 * This is similar to mcSimpleVertexClosure() except instead of returning the
 * vertices we return the outermost edges.
 */
void mcSimpleBoundryEdges(unsigned int vertex, unsigned int cube,
    unsigned int *edges, unsigned int *numEdges);

/**
 * This routine determines the XYZ-position of the given vertex relative to the
 * origin of the cube.
 */
void mcSimpleVertexRelativePosition(unsigned int vertex, unsigned int *pos);
