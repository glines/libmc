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
#include <math.h>
#include <stdio.h>  /* XXX */
#include <stdlib.h>

#include <mc/isosurfaceBuilder.h>
#include <mc/mesh.h>

#include "simple_table.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

/**
 * This file implements the simple marching cubes algorithm as described by
 * FIXME.
 */

void mcSimple_isosurfaceFromLattice(mcScalarLattice sl) {
}

void mcSimple_isosurfaceFromField(mcScalarField sf, mcMesh *mesh) {
  mcMesh_grow(mesh);  /* XXX */
  /* TODO: Pass the grid resolution as a parameter */
  int res = 10;
  float delta = 1.0f / (float)res;
  /* TODO: Iterate over the voxel grid to gather samples */
  float *samples = (float*)malloc(sizeof(float) * res * res * res);
  float d = 1.0f / (float)res;
  for (int z = 0; z < res; ++z) {
    for (int y = 0; y < res; ++y) {
      for (int x = 0; x < res; ++x) {
        int i = x + y * res + z * res * res;
        /* Add to the grid of samples */
        samples[i] = sf((float)x * d, (float)y * d, (float)z * d);
      }
    }
  }
  /* TODO: Iterate over the lattice structure */
  unsigned int *cubes = (unsigned int*)malloc(
      sizeof(unsigned int) * (res - 1) * (res - 1) * (res - 1));
  for (int z = 0; z < res - 1; ++z) {
    /* As the algorithm iterates along the z-axis, a 2-dimesnsional buffer of
     * the edge interpolation results from the previous slice is kept. This
     * allows the algorithm to take advantage of slice-to-slice coherence to
     * reduce the number of interpolation calculations required, as described
     * in "5.1 Efficiency Enhancements" in the original Marching Cubes paper by
     * Lorensen. This eliminates four redundant interpolations per voxel cube.
     *
     * Incidentally, this enhancement is necessary in order to generate an
     * indexed mesh that shares vertices among faces.
     *
     * NOTE: The Lorensen paper recommends against storing results from the
     * previous slice. There are two reasons this recommendation can be
     * ignored: First, computer memory has become much cheaper and more
     * abundant (the original paper was written in 1987). Second, the memory
     * requirements can be mitigated with a divide and conquer approach in
     * which the volume is divided into smaller volumes before the marching
     * cubes algorithm is applied. This divide and conquer approach lends
     * itself to parallelism as well.
     *
     * TODO: Actually implement this buffer.
     */
    for (int y = 0; y < res - 1; ++y) {
      /* As in the z-axis loop, the algorithm keeps a 1-dimensional buffer of
       * the edge interpolation results from the previous line. This eliminates
       * three redundant interpolations per voxel cube.
       */
      for (int x = 0; x < res - 1; ++x) {
        /* As in the z-axis and y-axis loops, the algorithm keeps a
         * 0-dimensional buffer of the edge interpolation results from the
         * previous voxel. This eliminates two redundant interpolations per
         * voxel cube.
         */
        int i = x + y * res + z * res * res;
        /* Determine the cube configuration index by iterating over the eight
         * cube vertices */
        unsigned int cube = 0;
        for (unsigned int vertex = 0; vertex < 8; ++vertex) {
          /* Determine this vertex's position in the lattice */
          unsigned int pos[3];
          unsigned int latticePos;
          mcSimpleVertexRelativePosition(vertex, pos);
          latticePos = i + pos[0] + pos[1] * res + pos[2] * res * res;
          /* Add the bit this vertex contributes to the cube */
          cube |= (samples[latticePos] >= 0.0f ? 1 : 0) << vertex;
          fprintf(stderr, "sample: %g\n", samples[latticePos]);  /* XXX */
        }
        fprintf(stderr, "voxel cube: 0x%02x\n", cube);  /* XXX */
        /* Look in the edge table for the edges that intersect the
         * isosurface */
        fprintf(stderr, "edges: (%d, %d, %d, ...)\n",
            mcSimpleEdgeTable[cube].edges[0],
            mcSimpleEdgeTable[cube].edges[1],
            mcSimpleEdgeTable[cube].edges[2]);  /* XXX */
        unsigned int numEdgeIntersections = 0;
        for (unsigned int j = 0;
            j < MC_SIMPLE_MAX_EDGES && mcSimpleEdgeTable[cube].edges[j] != -1;
            ++j)
        {
          unsigned int vertices[2];
          float values[2];
          int edge;
          mcVec3 latticePos[2];
          numEdgeIntersections += 1;
          edge = mcSimpleEdgeTable[cube].edges[j];
          mcSimpleEdgeVertices(edge, vertices);
          fprintf(stderr, "edge: %d, vertices: %d, %d\n",
              edge,
              vertices[0], 
              vertices[1]
              );
          /* Determine the value of each edge vertex */
          for (unsigned int k = 0; k < 2; ++k) {
            unsigned int pos[3];
            unsigned int index;
            mcSimpleVertexRelativePosition(vertices[k], pos);
            index = i + pos[0] + res * pos[1] + res * res * pos[2];
            values[k] = samples[index];
            latticePos[k].x = x + pos[0] * delta;
            latticePos[k].y = y + pos[1] * delta;
            latticePos[k].z = z + pos[2] * delta;
          }
          fprintf(stderr, "values: %g, %g\n",
              values[0],
              values[1]
              );
          /* Interpolate between vertex values */
          float weight = fabs(values[1] / (values[0] - values[1]));
          fprintf(stderr, "weight: %g\n", weight);
          /* The corresponding edge vertex must lie on the edge between the
           * lattice points, so we interpolate between these points. */
          mcVertex vertex;
          vertex.pos = mcVec3_lerp(&latticePos[0], &latticePos[1], weight);
          mcMesh_addVertex(mesh, &vertex);
        }
        /* Look in the triangulation table for the triangles corresponding to
         * this cube configuration. The number of edge intersections determines
         * the number of resulting triangles. */
        for (unsigned int j = 0; j < max(numEdgeIntersections - 2, 0); ++j) {
          mcSimpleTriangle triangle =
            mcSimpleTriangulationTable[cube].triangles[j];
          fprintf(stderr, "triangle: %d, %d, %d\n",
              triangle.edges[0],
              triangle.edges[1],
              triangle.edges[2]);
        }
      }
    }
  }
}
