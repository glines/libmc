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

void mcSimple_isosurfaceFromField(
    mcScalarFieldWithArgs sf, const void *args, mcMesh *mesh)
{
  /* TODO: Pass the grid resolution as a parameter */
  /* TODO: Support lattice structures that are not perfectly cube */
  /* TODO: Support sub-lattice structures (for divide-and-conquer) */
  int res = 10;
  float delta = 1.0f / (float)res;
  /* As the algorithm iterates along the z-axis, a 2-dimesnsional buffer
   * (called prevSlice) of the edge interpolation results from the previous
   * slice is kept. This allows the algorithm to take advantage of
   * slice-to-slice coherence to reduce the number of interpolation
   * calculations required, as described in "5.1 Efficiency Enhancements" in
   * the original Marching Cubes paper by Lorensen. This eliminates on average
   * four redundant interpolations per voxel cube.
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
   */
  typedef struct PrevSliceVoxel {
    /* FIXME: These edges should be numbered relative to the prev slice */
    unsigned int e0, e1, e2, e3;
  } PrevSliceVoxel;
  PrevSliceVoxel *prevSlice =
    (PrevSliceVoxel*)malloc(sizeof(PrevSliceVoxel) * res * res);
  /* As in the z-axis loop, the algorithm keeps a 1-dimensional buffer (called
   * prevLine) of the edge interpolation results from the previous line. This
   * eliminates on average three redundant interpolations per voxel cube.
   */
  typedef struct PrevLineVoxel {
    unsigned int e0, e4, e8, e9;
  } PrevLineVoxel;
  PrevLineVoxel *prevLine =
    (PrevLineVoxel*)malloc(sizeof(PrevLineVoxel) * res);
  /* As in the z-axis and y-axis loops, the algorithm keeps a 0-dimensional
   * buffer of the edge interpolation results from the previous voxel. This
   * eliminates on average two redundant interpolations per voxel cube.
   */
  typedef struct PrevVoxel {
    unsigned int e3, e7, e8, e10;
  } PrevVoxel;
  PrevVoxel prevVoxel;
  /* Iterate over the sample lattice */
  for (int z = 0; z < res - 1; ++z) {
    for (int y = 0; y < res - 1; ++y) {
      for (int x = 0; x < res - 1; ++x) {
        int i = x + y * res + z * res * res;
        /* Determine the cube configuration index by iterating over the eight
         * cube vertices */
        unsigned int cube = 0;
        for (unsigned int vertex = 0; vertex < 8; ++vertex) {
          /* Determine this vertex's relative position in the cube */
          unsigned int pos[3];
          float sample;
          mcSimpleVertexRelativePosition(vertex, pos);
          /* TODO: Many of these sample values can be stored/retrieved from a cache */
          sample = sf(
              (x + pos[0]) * delta,
              (y + pos[1]) * delta,
              (z + pos[2]) * delta,
              args);
          /* Add the bit this vertex contributes to the cube */
          cube |= (sample >= 0.0f ? 1 : 0) << vertex;
          fprintf(stderr, "sample: %g\n", sample);  /* XXX */
        }
        fprintf(stderr, "voxel cube: 0x%02x\n", cube);  /* XXX */
        /* Look in the edge table for the edges that intersect the
         * isosurface */
        fprintf(stderr, "edges: (%d, %d, %d, ...)\n",
            mcSimpleEdgeTable[cube].edges[0],
            mcSimpleEdgeTable[cube].edges[1],
            mcSimpleEdgeTable[cube].edges[2]);  /* XXX */
        unsigned int vertexIndices[MC_SIMPLE_MAX_EDGES];
        int numEdgeIntersections = 0;
        for (unsigned int j = 0;
            j < MC_SIMPLE_MAX_EDGES && mcSimpleEdgeTable[cube].edges[j] != -1;
            ++j)
        {
          unsigned int vertices[2];
          float values[2];
          int edge;
          mcVec3 latticePos[2];
          edge = mcSimpleEdgeTable[cube].edges[j];
          numEdgeIntersections += 1;
          /* Screen out edges that already have vertices and get their vertex
           * index from one of the prev voxel buffers */
          int skip = 0;
          switch (edge) {
            case 0:
              if (y > 0) {
                vertexIndices[edge] = prevLine[x].e0;
                skip = 1;
              }
              else if (z > 0) {
                vertexIndices[edge] = prevSlice[x + y * res].e0;
                skip = 1;
              }
              break;
            case 1:
              if (z > 0) {
                vertexIndices[edge] = prevSlice[x + y * res].e1;
                skip = 1;
              }
              break;
            case 2:
              if (z > 0) {
                vertexIndices[edge] = prevSlice[x + y * res].e2;
                skip = 1;
              }
              break;
            case 3:
              if (x > 0) {
                vertexIndices[edge] = prevVoxel.e3;
                skip = 1;
              }
              else if (z > 0) {
                vertexIndices[edge] = prevSlice[x + y * res].e3;
                skip = 1;
              }
              break;
            case 4:
              if (y > 0) {
                vertexIndices[edge] = prevLine[x].e4;
                skip = 1;
              }
              break;
            case 7:
              if (x > 0) {
                vertexIndices[edge] = prevVoxel.e7;
                skip = 1;
              }
              break;
            case 8:
              if (x > 0) {
                vertexIndices[edge] = prevVoxel.e8;
                skip = 1;
              }
              else if (y > 0) {
                vertexIndices[edge] = prevLine[x].e8;
                skip = 1;
              }
              break;
            case 9:
              if (y > 0) {
                vertexIndices[edge] = prevLine[x].e9;
                skip = 1;
              }
              break;
            case 10:
              if (x > 0) {
                vertexIndices[edge] = prevVoxel.e10;
                skip = 1;
              }
              break;
          }
          /*
          if (skip)
            continue; */ /* FIXME: Re-enable this skip once we have more triangles */
          /* Determine the value of each edge vertex */
          mcSimpleEdgeVertices(edge, vertices);
          fprintf(stderr, "edge: %d, vertices: %d, %d\n",
              edge,
              vertices[0], 
              vertices[1]
              );
          for (unsigned int k = 0; k < 2; ++k) {
            unsigned int pos[3];
            mcSimpleVertexRelativePosition(vertices[k], pos);
            /* TODO: Many of these sample values can be stored/retrieved from a cache */
            latticePos[k].x = (float)(x + pos[0]) * delta;
            latticePos[k].y = (float)(y + pos[1]) * delta;
            latticePos[k].z = (float)(z + pos[2]) * delta;
            values[k] = sf(
                latticePos[k].x,
                latticePos[k].y,
                latticePos[k].z,
                args);
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
          /* Add this vertex to the mesh */
          vertexIndices[edge] = mcMesh_addVertex(mesh, &vertex);
          /* Add the index for this vertex to the appropriate prev voxel
           * buffers */
          /* TODO: Facilitate debugging here by setting all unused edge indices to -1.
           */
          switch (edge) {
            case 1:
              prevVoxel.e3 = vertexIndices[edge];
              break;
            case 2:
              prevLine[x].e0 = vertexIndices[edge];
              break;
            case 4:
              prevSlice[x + y * res].e0 = vertexIndices[edge];
              break;
            case 5:
              prevVoxel.e7 = vertexIndices[edge];
              prevSlice[x + y * res].e1 = vertexIndices[edge];
              break;
            case 6:
              prevLine[x].e4 = vertexIndices[edge];
              prevSlice[x + y * res].e2 = vertexIndices[edge];
              break;
            case 7:
              prevSlice[x + y * res].e3 = vertexIndices[edge];
              break;
            case 9:
              prevVoxel.e8 = vertexIndices[edge];
              break;
            case 10:
              prevLine[x].e8 = vertexIndices[edge];
              break;
            case 11:
              prevVoxel.e10 = vertexIndices[edge];
              prevLine[x].e9 = vertexIndices[edge];
              break;
          }
        }
        /* Look in the triangulation table for the triangles corresponding to
         * this cube configuration. The number of edge intersections determines
         * the number of resulting triangles. */
        for (int j = 0; j < max(numEdgeIntersections - 2, 0); ++j) {
          mcFace face;
          mcFace_init(&face, 3);
          fprintf(stderr, "num edges: %d\n", numEdgeIntersections);
          /* FIXME: Re-enable this assertion */
          /* assert(mcSimpleTriangulationTable[cube].triangles[j].edges[0] != -1); */
          if (mcSimpleTriangulationTable[cube].triangles[j].edges[0] == -1)  /* XXX */
            break;  /* XXX */
          mcSimpleTriangle triangle =
            mcSimpleTriangulationTable[cube].triangles[j];
          fprintf(stderr, "triangle: %d, %d, %d\n",
              triangle.edges[0],
              triangle.edges[1],
              triangle.edges[2]);
          face.indices[0] = vertexIndices[triangle.edges[0]];
          face.indices[1] = vertexIndices[triangle.edges[1]];
          face.indices[2] = vertexIndices[triangle.edges[2]];
          mcMesh_addFace(mesh, &face);
          mcFace_destroy(&face);
        }
      }
    }
  }
  free(prevLine);
  free(prevSlice);
}
