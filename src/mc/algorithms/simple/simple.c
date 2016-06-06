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
#include <stdlib.h>

#include <mc/algorithms/common/cube.h>
#include <mc/isosurfaceBuilder.h>
#include <mc/mesh.h>

#include "simple_table.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

/**
 * This file implements the simple marching cubes algorithm as described by
 * Lorensen in "Marching Cubes: A high Resolution 3D Surface Construction
 * Algorithm"
 */

void mcSimple_isosurfaceFromLattice(mcScalarLattice sl) {
}

void mcSimple_isosurfaceFromField(
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int x_res, unsigned int y_res, unsigned int z_res,
    const mcVec3 *min, const mcVec3 *max,
    mcMesh *mesh)
{
  /* TODO: Pass the grid resolution as a parameter */
  /* TODO: Support lattice structures that are not perfectly cube */
  /* TODO: Support sub-lattice structures (for divide-and-conquer) */
  float delta_x = fabs(max->x - min->x) / (float)(x_res - 1);
  float delta_y = fabs(max->y - min->y) / (float)(y_res - 1);
  float delta_z = fabs(max->z - min->z) / (float)(z_res - 1);
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
    (PrevSliceVoxel*)malloc(sizeof(PrevSliceVoxel) * x_res * y_res);
  /* As in the z-axis loop, the algorithm keeps a 1-dimensional buffer (called
   * prevLine) of the edge interpolation results from the previous line. This
   * eliminates on average three redundant interpolations per voxel cube.
   */
  typedef struct PrevLineVoxel {
    unsigned int e0, e4, e8, e9;
  } PrevLineVoxel;
  PrevLineVoxel *prevLine =
    (PrevLineVoxel*)malloc(sizeof(PrevLineVoxel) * x_res);
  /* As in the z-axis and y-axis loops, the algorithm keeps a 0-dimensional
   * buffer of the edge interpolation results from the previous voxel. This
   * eliminates on average two redundant interpolations per voxel cube.
   */
  typedef struct PrevVoxel {
    unsigned int e3, e7, e8, e10;
  } PrevVoxel;
  PrevVoxel prevVoxel;
  /* A sample buffer of three slices is needed in order to calculate the vertex
   * normals. The buffer must contain samples from the previous slice, samples
   * for the current slice, and samples for the next slice. We store these
   * slices in a circular buffer. */
  /* FIXME: This buffer might need to be four slices large for computing
   * normals. See Lorensen. */
  float *samples = (float*)malloc(sizeof(float) * x_res * y_res * 3);
  int currentSlice = 2;
  /* Initialize the sample buffer */
  for (int z = 0; z < 2; ++z) {
    for (int y = 0; y < y_res; ++y) {
      for (int x = 0; x < x_res; ++x) {
        int i = x + y * x_res + z * x_res * y_res;
        samples[i] = sf(
            min->x + (float)x * delta_x,
            min->y + (float)y * delta_y,
            min->z + (float)z * delta_z,
            args);
      }
    }
  }
  /* Iterate over the cube lattice */
  for (int z = 0; z < z_res - 1; ++z) {
    /* Rotate the sample buffer and get samples for next slice */
    currentSlice = (currentSlice + 1) % 3;
    for (int y = 0; y < y_res; ++y) {
      for (int x = 0; x < x_res; ++x) {
        int i = x + y * x_res + ((currentSlice + 2) % 3) * x_res * y_res;
        if (z + 2 == z_res) {
          /* FIXME: Don't sample past min->z + (z_res - 1) * delta_z */
        }
        samples[i] = sf(
            min->x + (float)x * delta_x,
            min->y + (float)y * delta_y,
            min->z + (float)(z + 2) * delta_z,
            args);
      }
    }
    for (int y = 0; y < y_res - 1; ++y) {
      for (int x = 0; x < x_res - 1; ++x) {
        /* Determine the cube configuration index by iterating over the eight
         * cube vertices */
        unsigned int cube = 0;
        for (unsigned int vertex = 0; vertex < 8; ++vertex) {
          unsigned int pos[3];
          unsigned int i;
          /* Determine this vertex's relative position in the cube and sample
           * buffer */
          mcCube_vertexRelativePosition(vertex, pos);
          i = x + pos[0]
              + (y + pos[1]) * x_res
              + ((currentSlice + pos[2]) % 3) * x_res * y_res;
          assert(
              samples[i] == sf(
                min->x + (x + pos[0]) * delta_x,
                min->y + (y + pos[1]) * delta_y,
                min->z + (z + pos[2]) * delta_z,
                args));
          /* Add the bit this vertex contributes to the cube */
          cube |= (samples[i] >= 0.0f ? 1 : 0) << vertex;
        }
        /* Look in the edge table for the edges that intersect the
         * isosurface */
        unsigned int vertexIndices[MC_CUBE_NUM_EDGES];
        int numEdgeIntersections = 0;
        for (unsigned int j = 0;
            j < MC_CUBE_NUM_EDGES && mcSimple_edgeTable[cube].edges[j] != -1;
            ++j)
        {
          unsigned int vertices[2];
          float values[2];
          int edge;
          mcVec3 latticePos[2];
          edge = mcSimple_edgeTable[cube].edges[j];
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
                vertexIndices[edge] = prevSlice[x + y * x_res].e0;
                skip = 1;
              }
              break;
            case 1:
              if (z > 0) {
                vertexIndices[edge] = prevSlice[x + y * x_res].e1;
                skip = 1;
              }
              break;
            case 2:
              if (z > 0) {
                vertexIndices[edge] = prevSlice[x + y * x_res].e2;
                skip = 1;
              }
              break;
            case 3:
              if (x > 0) {
                vertexIndices[edge] = prevVoxel.e3;
                skip = 1;
              }
              else if (z > 0) {
                vertexIndices[edge] = prevSlice[x + y * x_res].e3;
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
          mcCube_edgeVertices(edge, vertices);
          for (unsigned int k = 0; k < 2; ++k) {
            unsigned int pos[3];
            mcCube_vertexRelativePosition(vertices[k], pos);
            /* TODO: Many of these sample values can be stored/retrieved from a cache */
            latticePos[k].x = min->x + (float)(x + pos[0]) * delta_x;
            latticePos[k].y = min->y + (float)(y + pos[1]) * delta_y;
            latticePos[k].z = min->z + (float)(z + pos[2]) * delta_z;
            values[k] = sf(
                latticePos[k].x,
                latticePos[k].y,
                latticePos[k].z,
                args);
          }
          /* Interpolate between vertex values */
          float weight = fabs(values[0] / (values[0] - values[1]));
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
              prevSlice[x + y * x_res].e0 = vertexIndices[edge];
              break;
            case 5:
              prevVoxel.e7 = vertexIndices[edge];
              prevSlice[x + y * x_res].e1 = vertexIndices[edge];
              break;
            case 6:
              prevLine[x].e4 = vertexIndices[edge];
              prevSlice[x + y * x_res].e2 = vertexIndices[edge];
              break;
            case 7:
              prevSlice[x + y * x_res].e3 = vertexIndices[edge];
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
         * this cube configuration. */
        for (int j = 0; j < MC_SIMPLE_MAX_TRIANGLES; ++j) {
          mcFace face;
          if (mcSimple_triangulationTable[cube].triangles[j].edges[0] == -1)
            break;  /* No more triangles */
          mcFace_init(&face, 3);
          mcSimpleTriangle triangle =
            mcSimple_triangulationTable[cube].triangles[j];
          face.indices[0] = vertexIndices[triangle.edges[0]];
          face.indices[1] = vertexIndices[triangle.edges[1]];
          face.indices[2] = vertexIndices[triangle.edges[2]];
          mcMesh_addFace(mesh, &face);
          mcFace_destroy(&face);
        }
      }
    }
  }
  free(samples);
  free(prevLine);
  free(prevSlice);
}
