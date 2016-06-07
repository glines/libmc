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
#include <string.h>

#include <mc/algorithms/common/cube.h>
#include <mc/isosurfaceBuilder.h>
#include <mc/mesh.h>

#include "simple_table.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

#define mod(a, b) ((a) % (b) < 0 ? (a) % (b) + (b) : (a) % (b))

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
  typedef struct SliceVoxel {
    int e2, e6, e10, e11;
  } SliceVoxel;
  SliceVoxel *previousSlice =
    (SliceVoxel*)malloc(sizeof(SliceVoxel) * (x_res - 1) * (y_res - 1));
  SliceVoxel *currentSlice =
    (SliceVoxel*)malloc(sizeof(SliceVoxel) * (x_res - 1) * (y_res - 1));
  /* As in the z-axis loop, the algorithm keeps a 1-dimensional buffer (called
   * prevLine) of the edge interpolation results from the previous line. This
   * eliminates on average three redundant interpolations per voxel cube.
   */
  typedef struct LineVoxel {
    int e4, e5, e6, e7;
  } LineVoxel;
  LineVoxel *previousLine =
    (LineVoxel*)malloc(sizeof(LineVoxel) * (x_res - 1));
  LineVoxel *currentLine =
    (LineVoxel*)malloc(sizeof(LineVoxel) * (x_res - 1));
  /* As in the z-axis and y-axis loops, the algorithm keeps a 0-dimensional
   * buffer of the edge interpolation results from the previous voxel. This
   * eliminates on average two redundant interpolations per voxel cube.
   */
  typedef struct Voxel {
    int e1, e5, e9, e11;
  } Voxel;
  Voxel *previousVoxel = (Voxel*)malloc(sizeof(Voxel));
  Voxel *currentVoxel = (Voxel*)malloc(sizeof(Voxel));
  /* A sample buffer of four slices is needed in order to calculate the vertex
   * normals. The buffer must contain samples from the current cube as well as
   * samples from slices before and after the current cube's samples. We store
   * these slices in a circular buffer. The current sample slice index always
   * points to the slice containing the sample for the zero vertex on the
   * current cube.
   */
  float *samples = (float*)malloc(sizeof(float) * x_res * y_res * 4);
  int sampleSliceIndex = 3;
  /* Initialize the sample buffer */
  for (int z = 0; z < 4; ++z) {  /* XXX: z should only need to go to 1 */
    for (int y = 0; y < y_res; ++y) {
      for (int x = 0; x < x_res; ++x) {
        int i = x + y * x_res + z * x_res * y_res;
        samples[i] = sf(
            min->x + (float)x * delta_x,
            min->y + (float)y * delta_y,
            min->z + (float)z * delta_z,
            args);
        if (z == 2) {  /* XXX */
          samples[i] = 1000.0f;
        }
        if (z == 3) {  /* XXX */
          samples[i] = 1000.0f;
        }
      }
    }
  }
  /* Iterate over the cube lattice */
  for (int z = 0; z < z_res - 1; ++z) {
    /* Rotate the sample buffer and get samples for next slice */
    sampleSliceIndex = (sampleSliceIndex + 1) % 4;
    if (z + 2 < z_res) {  /* Don't sample past the maximum resolution */
      for (int y = 0; y < y_res; ++y) {
        for (int x = 0; x < x_res; ++x) {
          int i = x + y * x_res + ((sampleSliceIndex + 2) % 4) * x_res * y_res;
          samples[i] = sf(
              min->x + (float)x * delta_x,
              min->y + (float)y * delta_y,
              min->z + (float)(z + 2) * delta_z,
              args);
        }
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
              + ((sampleSliceIndex + pos[2]) % 4) * x_res * y_res;
#ifndef __EMSCRIPTEN__
          /* NOTE: This assertion does not work with Emscripten for
           * some reason */
          assert(samples[i] == sf(min->x + (x + pos[0]) * delta_x,
                                  min->y + (y + pos[1]) * delta_y,
                                  min->z + (z + pos[2]) * delta_z,
                                  args));
#endif
          /* Add the bit this vertex contributes to the cube */
          cube |= (samples[i] >= 0.0f ? 1 : 0) << vertex;
        }
        /* Look in the edge table for the edges that intersect the
         * isosurface */
        int vertexIndices[MC_CUBE_NUM_EDGES];
        /* The edges in the previous voxel buffers need to be reset to -1 if
         * the given edge is not in the edge table for the given cube
         * configuration. This is easy since the edge table is already sorted.
         * We iterate over all possible edges, and for any edges we do not find
         * in the table we leave their vertex index value at -1. */
        int edgeTableIndex = 0;
        /* TODO: Most of this loop can be avoided in a release build by simply
         * moving from one edge in mcSimple_edgeTable[cube] to the next rather
         * than iterating over all possible edges. */
        for (int edge = 0; edge < MC_CUBE_NUM_EDGES; ++edge) {
          vertexIndices[edge] = -1;
          if (mcSimple_edgeTable[cube].edges[edgeTableIndex] == edge) {
            /* This edge intersection must exist. We will either find it in one
             * of our buffers or compute it ourselves. */
            /* Look for edge intersections that already have vertices and get
             * their vertex index from one of the previous voxel buffers */
            switch (edge) {
              case 0:
                if (y > 0) {
                  vertexIndices[edge] = previousLine[x].e4;
                  assert(vertexIndices[edge] != -1);
                }
                else if (z > 0) {
                  vertexIndices[edge] = previousSlice[x + y * (x_res - 1)].e2;
                  assert(vertexIndices[edge] != -1);
                }
                break;
              case 1:
                if (y > 0) {
                  vertexIndices[edge] = previousLine[x].e5;
                  assert(vertexIndices[edge] != -1);
                }
                break;
              case 2:
                if (y > 0) {
                  vertexIndices[edge] = previousLine[x].e6;
                  assert(vertexIndices[edge] != -1);
                }
                break;
              case 3:
                if (x > 0) {
                  vertexIndices[edge] = previousVoxel->e1;
                  assert(vertexIndices[edge] != -1);
                }
                else if (y > 0) {
                  vertexIndices[edge] = previousLine[x].e7;
                  assert(vertexIndices[edge] != -1);
                }
                break;
              case 4:
                if (z > 0) {
                  vertexIndices[edge] = previousSlice[x + y * (x_res - 1)].e6;
                  assert(vertexIndices[edge] != -1);
                }
                break;
              case 7:
                if (x > 0) {
                  vertexIndices[edge] = previousVoxel->e5;
                  assert(vertexIndices[edge] != -1);
                }
                break;
              case 8:
                if (x > 0) {
                  vertexIndices[edge] = previousVoxel->e9;
                  assert(vertexIndices[edge] != -1);
                }
                else if (z > 0) {
                  vertexIndices[edge] = previousSlice[x + y * (x_res - 1)].e10;
                  assert(vertexIndices[edge] != -1);
                }
                break;
              case 9:
                if (z > 0) {
                  vertexIndices[edge] = previousSlice[x + y * (x_res - 1)].e11;
                  assert(vertexIndices[edge] != -1);
                }
                break;
              case 10:
                if (x > 0) {
                  vertexIndices[edge] = previousVoxel->e11;
                  assert(vertexIndices[edge] != -1);
                }
                break;
            }
            if (vertexIndices[edge] == -1) {
              /* The mesh vertex for this edge intersection has not been generated yet */
              unsigned int vertices[2];
              float values[2];
              mcVec3 latticePos[2];
              mcVec3 gradiants[2];
              /* Find the cube vertices on this edge */
              mcCube_edgeVertices(edge, vertices);
              for (unsigned int i = 0; i < 2; ++i) {
                unsigned int pos[3], abs[3];
                int j;
                mcCube_vertexRelativePosition(vertices[i], pos);
                abs[0] = x + pos[0];
                abs[1] = y + pos[1];
                abs[2] = z + pos[2];
                latticePos[i].x = min->x + (float)(abs[0]) * delta_x;
                latticePos[i].y = min->y + (float)(abs[1]) * delta_y;
                latticePos[i].z = min->z + (float)(abs[2]) * delta_z;
                /* Find the sample in our samples buffer */
                j = abs[0]
                    + abs[1] * x_res
                    + ((sampleSliceIndex + pos[2]) % 4) * x_res * y_res;
#ifndef __EMSCRIPTEN__
                /* NOTE: This assertion does not work with Emscripten for
                 * some reason */
                assert(samples[j] == sf(latticePos[i].x,
                                        latticePos[i].y,
                                        latticePos[i].z,
                                        args));
#endif
                values[i] = samples[j];
                /* Calculate the surface normal by estimating the gradiant of
                 * the scalar field at the cube vertices, and then
                 * interpolating between the two gradiants. (see Lorensen,
                 * "Marching Cubes: A High Resolution 3D Surface Construction
                 * Algorihm") */
                /* FIXME: I'm not so sure delta_x is the correct divisor */
                gradiants[i].x =
                  (samples[abs[0] + ((abs[0] < x_res - 1) ? 1 : 0)
                           + abs[1] * x_res
                           + ((sampleSliceIndex + pos[2]) % 4) * x_res * y_res]
                   - samples[abs[0] - (abs[0] > 0 ? 1 : 0)
                             + abs[1] * x_res
                             + ((sampleSliceIndex + pos[2]) % 4) * x_res * y_res]
                  ) / delta_x;
                /* FIXME: I'm not so sure delta_y is the correct divisor */
                gradiants[i].y =
                  (samples[abs[0]
                           + (abs[1] + ((abs[1] < y_res - 1) ? 1 : 0)) * x_res
                           + ((sampleSliceIndex + pos[2]) % 4) * x_res * y_res]
                   - samples[abs[0]
                             + (abs[1] - (abs[1] > 0 ? 1 : 0)) * x_res
                             + ((sampleSliceIndex + pos[2]) % 4) * x_res * y_res]
                  ) / delta_y;
                /* FIXME: I'm not so sure delta_z is the correct divisor */
                gradiants[i].z =
                  (samples[abs[0]
                           + abs[1] * x_res
                           + ((sampleSliceIndex + pos[2] + ((abs[2] < z_res - 1) ? 1 : 0)) % 4)
                             * x_res * y_res]
                   - samples[abs[0]
                             + abs[1] * x_res
                             + mod((sampleSliceIndex + pos[2] - (abs[2] > 0 ? 1 : 0)), 4)
                               * x_res * y_res]
                  ) / delta_z;
              }
              /* Interpolate between the sample values at each vertex */
              float weight = fabs(values[0] / (values[0] - values[1]));
              /* The corresponding edge vertex must lie on the edge between the
               * lattice points, so we interpolate between these points. */
              mcVertex vertex;
              vertex.pos = mcVec3_lerp(&latticePos[0], &latticePos[1], weight);
              /* Interpolate between the gradiants to approximate the surface
               * normal */
              vertex.norm = mcVec3_lerp(&gradiants[0], &gradiants[1], weight);
              mcVec3_normalize(&vertex.norm);
              /* Add this vertex to the mesh */
              vertexIndices[edge] = mcMesh_addVertex(mesh, &vertex);
            }
            edgeTableIndex += 1;
          }
          /* Add the index for this vertex to the appropriate prev voxel
           * buffers so we can connect the mesh properly. */
          switch (edge) {
            case 1:
              currentVoxel->e1 = vertexIndices[edge];
              break;
            case 2:
              currentSlice[x + y * (x_res - 1)].e2 = vertexIndices[edge];
              break;
            case 4:
              currentLine[x].e4 = vertexIndices[edge];
              break;
            case 5:
              currentVoxel->e5 = vertexIndices[edge];
              currentLine[x].e5 = vertexIndices[edge];
              break;
            case 6:
              currentLine[x].e6 = vertexIndices[edge];
              currentSlice[x + y * (x_res - 1)].e6 = vertexIndices[edge];
              break;
            case 7:
              currentLine[x].e7 = vertexIndices[edge];
              break;
            case 9:
              currentVoxel->e9 = vertexIndices[edge];
              break;
            case 10:
              currentSlice[x + y * (x_res - 1)].e10 = vertexIndices[edge];
              break;
            case 11:
              currentVoxel->e11 = vertexIndices[edge];
              currentSlice[x + y * (x_res - 1)].e11 = vertexIndices[edge];
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
        /* Make the current voxel the previous one */
        Voxel *temp = previousVoxel;
        previousVoxel = currentVoxel;
        currentVoxel = temp;
      }
      /* Make the current line the previous one */
      LineVoxel *temp = previousLine;
      previousLine = currentLine;
      currentLine = temp;
    }
    /* Make the current slice the previous one */
    SliceVoxel *temp = previousSlice;
    previousSlice = currentSlice;
    currentSlice = temp;
  }
  free(samples);
  free(previousVoxel);
  free(currentVoxel);
  free(previousLine);
  free(currentLine);
  free(previousSlice);
  free(currentSlice);
}
