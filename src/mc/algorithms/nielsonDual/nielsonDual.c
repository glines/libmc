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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mc/algorithms/common/cube.h>

#include <mc/algorithms/nielsonDual/common.h>
#include <mc/algorithms/nielsonDual/nielsonDual.h>
#include <mc/algorithms/nielsonDual/nielsonDual_tables.h>

#include "nielsonDual_tables.c"

/* TODO: Support splatting algorithm as described by Nielson. This is the same
 * algorithm except instead of connecting the mesh topology we simply render
 * the quad patch generated and move on.
 */

/**
 * This routine implements the MC-Dual isosurface extraction algorithm as
 * described by Nielson in "Dual Marching Cubes." This does not implement the
 * Dual-of-the-Dual operator.
 */
void mcNielsonDual_isosurfaceFromField(
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int x_res, unsigned int y_res, unsigned int z_res,
    const mcVec3 *min, const mcVec3 *max,
    mcMesh *mesh)
{
  mcFace triangle, quad;
  mcFace_init(&triangle, 3);
  mcFace_init(&quad, 4);
  float delta_x = fabs(max->x - min->x) / (float)(x_res - 1);
  float delta_y = fabs(max->y - min->y) / (float)(y_res - 1);
  float delta_z = fabs(max->z - min->z) / (float)(z_res - 1);
  /* We allocate some buffers to facilitate constructing mesh topology.
   *
   * Note that the buffers extend beyond the cube lattice structure specified
   * by the input parameters. This is because we use phantom sample points
   * above the isosurface for the edge cases to avoid the problem of
   * inaccessible buffers and nonmanifold geometry. */
  /* This struct defines the connecting vector interface between a voxel cube
   * and the voxel cube in the next slice. */
  typedef struct Voxel {
    /* NOTE: We should only need to store two vertex indices here since there
     * are only two possible vertices that interface through the top face, but
     * we allocate space for four so that we can use the
     * mcNielsonDual_vertexIndexLookupTable to quickly find the vertex index we
     * need. */
    int vertexIndices[MC_NIELSON_DUAL_MAX_VERTICES];
    int cube;
  } Voxel;
  Voxel *previousSlice = (Voxel*)malloc(
      sizeof(Voxel) * (x_res + 1) * (y_res + 1));
  Voxel *currentSlice = (Voxel*)malloc(
      sizeof(Voxel) * (x_res + 1) * (y_res + 1));
  Voxel *previousLine = (Voxel*)malloc(sizeof(Voxel) * (x_res + 1));
  Voxel *currentLine = (Voxel*)malloc(sizeof(Voxel) * (x_res + 1));
  Voxel *previousVoxel  = (Voxel*)malloc(sizeof(Voxel));
  Voxel *currentVoxel  = (Voxel*)malloc(sizeof(Voxel));
#ifndef NDEBUG
  /* Set canary values for our vertex indices */
  memset(previousSlice, -1, sizeof(Voxel) * (x_res + 1) * (y_res + 1));
  memset(previousLine, -1, sizeof(Voxel) * (x_res + 1));
  memset(previousVoxel, -1, sizeof(Voxel));
#endif
  /* Iterate over the cube lattice */
  for (int z = -1; z < (int)z_res; ++z) {
#ifndef NDEBUG
    /* Set canary values for our slice vertex indices */
    memset(currentSlice, -1, sizeof(Voxel) * (x_res + 1) * (y_res + 1));
#endif
    for (int y = -1; y < (int)y_res; ++y) {
#ifndef NDEBUG
      /* Set canary values for our line vertex indices */
      memset(currentLine, -1, sizeof(Voxel) * (x_res + 1));
#endif
      for (int x = -1; x < (int)x_res; ++x) {
#ifndef NDEBUG
        /* Set canary values for our voxel vertex indices */
        memset(currentVoxel, -1, sizeof(Voxel));
#endif
        /* Determine the cube configuration index by iterating over the eight
         * cube vertices */
        unsigned int cube = 0;
        for (unsigned int vertex = 0; vertex < 8; ++vertex) {
          int pos[3];
          float sample;
          /* Determine this vertex's relative position in the cube and sample
           * that point */
          /* TODO: Cache these sample values (see
           * src/mc/algorihms/patch/patch.c for example). */
          mcCube_vertexRelativePosition(vertex, (unsigned int*)pos);
          if ((x + pos[0] < 0) || (y + pos[1] < 0) || (z + pos[2] < 0)
              || (x + pos[0] >= x_res) || (y + pos[1] >= y_res) || (z + pos[2] >= z_res))
          {
            /* TODO: Fill the previous voxel buffers with cubes consistent with
             * a lattice grid that extends beyond the minimum and maximum
             * resolution resolutions, with samples beyond the resolutions
             * always above the isosurface.
             *
             * NOTE: We would also need to fudge voxel cubes past the maximum
             * resolution by looping to the very edge of the sample lattice.
             * Without doing so, a lot of the samples at the edges would be
             * ignored.
             *
             * NOTE: What if the user wants non-manifold geometry?
             */
            sample = 1.0f;
          } else {
            sample = sf(min->x + (x + pos[0]) * delta_x,
                min->y + (y + pos[1]) * delta_y,
                min->z + (z + pos[2]) * delta_z,
                args);
          }
          /* Add the bit this vertex contributes to the cube */
          cube |= (sample >= 0.0f ? 1 : 0) << vertex;
        }
        /* Store this cube configuration in our buffers */
        currentVoxel->cube = cube;
        currentLine[x + 1].cube = cube;
        currentSlice[(x + 1) + (y + 1) * (x_res + 1)].cube = cube;
        if (cube != 0x00 && cube != 0xff) { /* Skip the trivial cases */
          /* Look up vertices we need to generate for the given cube
           * configuration */
          const mcNielsonDualCookedVertexList *list =
            &mcNielsonDual_midpointVertexTable[cube];
          int vertexIndex;
          for (int i = 0; i < list->numVertices; ++i) {
            mcVertex vertex;
            /* NOTE: The table has enough information to know the exact position
             * of this vertex. We can add it to the mesh as is. The surface
             * normal can be estimated at this point by interpolation of lattice
             * gradiants, or even something simpler. */
            /* NOTE: The positions we compute are in mesh space coordinates,
             * not sample spac ecoordinates. The vertices of the mesh we
             * generate must be in mesh space coordinates in which min is at
             * the origin. */
            /* Compute the absolute position of this vertex */
            vertex.pos.x = ((float)x + list->vertices[i].pos.x) * delta_x;
            vertex.pos.y = ((float)y + list->vertices[i].pos.y) * delta_y;
            vertex.pos.z = ((float)z + list->vertices[i].pos.z) * delta_z;
            /* The normal is also retrieved from the table */
            /* TODO: Support computing more accurate normals from sample values */
            vertex.norm = list->vertices[i].norm;
            /* Add this vertex to the mesh */
            vertexIndex = mcMesh_addVertex(mesh, &vertex);
            /* Store this vertex index in our buffers */
            currentVoxel->vertexIndices[i] = vertexIndex;
            currentLine[x + 1].vertexIndices[i] = vertexIndex;
            currentSlice[(x + 1) + (y + 1) * (x_res + 1)].vertexIndices[i] = vertexIndex;
          }
          /* TODO: Iterate over the three edges for which we have generated
           * enough vertices to make its respective quad. */
          for (int i = 0; i < 3; ++i) {
            int edge, faces[2];
            int skip = 0;
            switch (i) {
              case 0:
                edge = 0;
                faces[0] = MC_CUBE_FACE_FRONT;
                faces[1] = MC_CUBE_FACE_BOTTOM;
                if (y < 0 || z < 0)
                  skip = 1;
                break;
              case 1:
                edge = 3;
                faces[0] = MC_CUBE_FACE_FRONT;
                faces[1] = MC_CUBE_FACE_RIGHT;
                if (x < 0 || y < 0)
                  skip = 1;
                break;
              case 2:
                edge = 8;
                faces[0] = MC_CUBE_FACE_BOTTOM;
                faces[1] = MC_CUBE_FACE_RIGHT;
                if (x < 0 || z < 0)
                  skip = 1;
                break;
            }
            if (skip)
              break;  /* Skip edge cases where we have not generated vertices in
                         the neighboring voxel */
            int lookupIndex, vertexIndices[4];
            Voxel *voxel;
            /* Get the vertex index for this edge */
            voxel = currentVoxel;
            lookupIndex = mcNielsonDual_vertexIndexLookupTable[
              (edge << 8) + cube];
            if (lookupIndex == -1) {
              /* This edge does not have an associated vertex, so it must not
               * intersect the isosurface. Skip this edge. */
              /* TODO: Add an assertion here that checks the sample values? */
              continue;
            }
            assert(voxel->vertexIndices[lookupIndex] != -1);
            vertexIndices[0] = voxel->vertexIndices[lookupIndex];
            /* Find the cubes near this edge for the two easy cases */
            for (int j = 0; j < 2; ++j) {
              switch (faces[j]) {
                case MC_CUBE_FACE_FRONT:
                  assert(y >= 0);
                  voxel = &previousLine[(x + 1)];
                  break;
                case MC_CUBE_FACE_BOTTOM:
                  assert(z >= 0);
                  voxel = &previousSlice[(x + 1) + (y + 1) * (x_res + 1)];
                  break;
                case MC_CUBE_FACE_RIGHT:
                  assert(x >= 0);
                  voxel = previousVoxel;
                  break;
              }
              /* Use the mcNielsonDual_vertexIndexLookupTable to find the vertex
               * indices for this edge and voxel cube configuration. */
              /* NOTE: The translated edge of the given face/edge combination is
               * the index of the given edge with respect to the voxel cube on
               * the other side of the given face. */
              lookupIndex = mcNielsonDual_vertexIndexLookupTable[
                (mcCube_translateEdge(edge, faces[j]) << 8) + voxel->cube];
              /* Find the vertex index for this voxel. This index must exist. */
              assert(lookupIndex != -1);
              assert(voxel->vertexIndices[lookupIndex] != -1);
              vertexIndices[j + 1] = voxel->vertexIndices[lookupIndex];
            }
            /* Find the voxel cube diagonal to this edge, which is somewhat more
             * complicated */
            switch (edge) {
              case 0:
                /* The diagonal cube is on the bottom-front */
                assert(y >= 0);
                assert(z >= 0);
                voxel = &previousSlice[(x + 1) + (y + 1 - 1) * (x_res + 1)];
                break;
              case 3:
                /* The diagonal cube is on the front-right */
                assert(x >= 0);
                assert(y >= 0);
                voxel = &previousLine[x + 1 - 1];
                break;
              case 8:
                /* The diagonal cube is on the bottom-right */
                assert(x >= 0);
                assert(z >= 0);
                voxel = &previousSlice[(x + 1 - 1) + (y + 1) * (x_res + 1)];
                break;
            }
            lookupIndex = mcNielsonDual_vertexIndexLookupTable[
              (mcCube_translateEdge(mcCube_translateEdge(edge, faces[0]), faces[1]) << 8)
                + voxel->cube];
            /* Find the vertex index for this voxel. This index must exist. */
            assert(lookupIndex != -1);
            assert(voxel->vertexIndices[lookupIndex] != -1);
            vertexIndices[3] = voxel->vertexIndices[lookupIndex];
            /* The signs of the samples on this edge to determine the correct
             * winding order. Enough information is available to quickly
             * determine the winding order from our winding order lookup table,
             * which returns the next face in the correct winding. */
            int winding = mcNielsonDual_windingTable[(edge << 8) + cube];
            quad.indices[0] = vertexIndices[0];
            quad.indices[2] = vertexIndices[3];
            if (winding == faces[0]) {
              quad.indices[1] = vertexIndices[1];
              quad.indices[3] = vertexIndices[2];
            } else {
              /* The winding table must agree with the faces adjacent to this
               * edge */
              assert(winding == faces[1]);
              quad.indices[1] = vertexIndices[2];
              quad.indices[3] = vertexIndices[1];
            }
            /* Add the quad to the mesh */
            mcMesh_addFace(mesh, &quad);
            /* TODO: Support triangulated meshes. */
            /* TODO: Determine the best triangulation based on angles. */
          }
        }
        /* Make the current voxel the previous one */
        Voxel *temp = previousVoxel;
        previousVoxel = currentVoxel;
        currentVoxel = temp;
      }
      /* Make the current line the previous one */
      Voxel *temp = previousLine;
      previousLine = currentLine;
      currentLine = temp;
    }
    /* Make the current slice the previous one */
    Voxel *temp = previousSlice;
    previousSlice = currentSlice;
    currentSlice = temp;
  }
  mcFace_destroy(&triangle);
  mcFace_destroy(&quad);
}
