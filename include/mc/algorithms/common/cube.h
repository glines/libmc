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

extern const unsigned int MC_CUBE_NUM_VERTICES;
extern const unsigned int MC_CUBE_NUM_EDGES;
extern const unsigned int MC_CUBE_NUM_FACES;

typedef struct mcCubeEdgeList {
  int edges[12];
} mcCubeEdgeList;

/**
 * This routine takes the index of an edge and returns the two vertex indices
 * on that edge in vertices.
 *
 * These indices are based on the numbering scheme described by the original
 * marching cubes paper by Lorensen.
 */
void mcCube_edgeVertices(unsigned int edge, unsigned int *vertices);

/**
 * This routine determines the two cube faces that the given edge lies on and
 * returns the indices of those faces in faces.
 */
void mcCube_edgeFaces(unsigned int edge, unsigned int *faces);

/**
 * This routine determines the edge that lies between the two given vertices
 * and returns that edge number. If the line between a and b is not incident to
 * the cube, or if a and b are the same vertex, then -1 is returned instead.
 */
int mcCube_verticesToEdge(unsigned int a, unsigned int b);

/**
 * This routine determines the edges connected to a given vertex.
 */
void mcCube_vertexEdges(unsigned int vertex, int *edges);

/**
 * This routine determines the vertices adjacent to the given vertex and
 * returns the adjacent vertex indices in adjacent.
 */
void mcCube_adjacentVertices(unsigned int vertex, unsigned int *adjacent);

/**
 * This routine determines the value of a vertex (either above or below the
 * isosurface) for the given numbered cube configuration.
 *
 * The returned value is 0 for less than the isosurface value, and 1 for
 * greater than or equal to the isosurface value.
 */
int mcCube_vertexValue(unsigned int vertex, unsigned int cube);

/**
 * This routine takes a vertex position on the unit cube with vertex 0
 * positioned at the origin and returns the vertex index.
 */
unsigned int mcCube_vertexIndex(
    unsigned int x, unsigned int y, unsigned int z);

/**
 * This routine traverses the edges of alike vertices to compute the closure of
 * a given vertex.
 */
void mcCube_vertexClosure(unsigned int vertex, unsigned int cube, 
    unsigned int *closure, unsigned int *closureSize);

/**
 * This starts at a vertex and traverses adjacent edges of alike vertices to
 * find the boundry edges on the cube.
 *
 * This is similar to mcSimpleVertexClosure() except instead of returning the
 * vertices we return the outermost edges.
 */
void mcCube_boundryEdges(unsigned int vertex, unsigned int cube,
    unsigned int *edges, unsigned int *numEdges);

/**
 * This routine determines the XYZ-position of the given vertex relative to the
 * origin of the cube.
 */
void mcCube_vertexRelativePosition(unsigned int vertex, unsigned int *pos);

/**
 * This routine determines the bitwise cube configuration from the given array
 * of eight vertex samples.
 *
 * The routine assumes that samples with value 0.0f lie on the positive side of
 * the isosurface, which is the assumption made by the classic marching cubes
 * algorithm.
 */
unsigned int mcCube_cubeConfigurationFromSamples(float *samples);
