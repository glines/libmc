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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
  mcFace triangle;
  mcFace_init(&triangle, 3);
  float delta_x = fabs(max->x - min->x) / (float)(x_res - 1);
  float delta_y = fabs(max->y - min->y) / (float)(y_res - 1);
  float delta_z = fabs(max->z - min->z) / (float)(z_res - 1);
  /* We allocate some buffers to facilitate constructing mesh topology */
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
  Voxel *prevSlice = (Voxel*)malloc(
      sizeof(Voxel) * (x_res - 1) * (y_res - 1));
  Voxel *currentSlice = (Voxel*)malloc(
      sizeof(Voxel) * (x_res - 1) * (y_res - 1));
  Voxel *prevLine = (Voxel*)malloc(sizeof(Voxel) * (x_res - 1));
  Voxel *currentLine = (Voxel*)malloc(sizeof(Voxel) * (x_res - 1));
  Voxel *prevVoxel  = (Voxel*)malloc(sizeof(Voxel));
  Voxel *currentVoxel  = (Voxel*)malloc(sizeof(Voxel));
  /* TODO: Fill the previous voxel buffers with cubes consisntet with a lattice
   * grid that extends beyond the minimum and maximum resolution resolutions,
   * with samples beyond the resolutions always above the isosurface.
   *
   * NOTE: We would also need to fudge voxel cubes past the maximum resolution
   * by looping to the very edge of the sample lattice. Without doing so, a lot
   * of the samples at the edges would be ignored.
   *
   * NOTE: What if the user wants non-manifold geometry?
   */
  /* TODO: Iterate over the cube lattice*/
  for (int z = 0; z < z_res; ++z) {
    for (int y = 0; y < y_res; ++y) {
      for (int x = 0; x < x_res; ++x) {
        /* Determine the cube configuration index by iterating over the eight
         * cube vertices */
        unsigned int cube = 0;
        for (unsigned int vertex = 0; vertex < 8; ++vertex) {
          unsigned int pos[3];
          float sample;
          /* Determine this vertex's relative position in the cube and sample
           * that point */
          /* TODO: Cache these sample values (see
           * src/mc/algorihms/patch/patch.c for example). */
          mcCube_vertexRelativePosition(vertex, pos);
          sample = sf(min->x + (x + pos[0]) * delta_x,
              min->y + (y + pos[1]) * delta_y,
              min->z + (z + pos[2]) * delta_z,
              args);
          /* Add the bit this vertex contributes to the cube */
          cube |= (sample >= 0.0f ? 1 : 0) << vertex;
        }
        fprintf(stderr, "cube: 0x%02x\n", cube);
        /* Store this cube configuration in our buffers */
        currentVoxel->cube = cube;
        currentLine[x].cube = cube;
        currentSlice[x + y * (x_res - 1)].cube = cube;
        if (cube == 0x00 || cube == 0xff)
          continue;  /* Skip the trivial cases */
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
          /* TODO: Compute the surface normal at this vertex */
          vertex.pos.x = min->x + ((float)x + list->vertices[i].pos.x) * delta_x;
          vertex.pos.y = min->y + ((float)y + list->vertices[i].pos.y) * delta_y;
          vertex.pos.z = min->z + ((float)z + list->vertices[i].pos.z) * delta_z;
          /* Add this vertex to the mesh */
          vertexIndex = mcMesh_addVertex(mesh, &vertex);
          /* Store this vertex index in our buffers */
          currentVoxel->vertexIndices[i] = vertexIndex;
          currentLine[x].vertexIndices[i] = vertexIndex;
          currentSlice[x + y * (x_res - 1)].vertexIndices[i] = vertexIndex;
          /* XXX: Draw a small triangle around this vertex for debugging
           * purposes */
          vertex.pos.x += delta_x * 0.1;
          mcMesh_addVertex(mesh, &vertex);
          vertex.pos.y += delta_y * 0.1;
          mcMesh_addVertex(mesh, &vertex);
          triangle.indices[0] = vertexIndex;
          triangle.indices[1] = vertexIndex + 1;
          triangle.indices[2] = vertexIndex + 2;
          mcMesh_addFace(mesh, &triangle);
        }
#if 0
        /* TODO: Iterate over the three edges for which we have generated
         * enough vertices to make its respective quad. */
        for (int i = 0; i < 3; ++i) {
          int edge, faces[2];
          switch (i) {
            case 0:
              edge = 0;
              faces[0] = MC_CUBE_FACE_FRONT;
              faces[1] = MC_CUBE_FACE_BOTTOM;
              break;
            case 1:
              edge = 3;
              faces[0] = MC_CUBE_FACE_FRONT;
              faces[1] = MC_CUBE_FACE_RIGHT;
              break;
            case 2:
              edge = 8;
              faces[0] = MC_CUBE_FACE_BOTTOM;
              faces[1] = MC_CUBE_FACE_RIGHT;
              break;
          }
          int lookupIndex, vertexIndices[4];
          Voxel *voxel;
          /* Get the vertex index for this edge */
          voxel = currentVoxel;
          lookupIndex = mcNielsonDual_vertexIndexLookupTable[
            (edge << 8) + cube];
          if (voxel->vertexIndices[lookupIndex] == -1) {
            /* This edge does not have an associated vertex, so it must not
             * intersect the isosurface. Skip this edge. */
            continue;
          }
          vertexIndices[0] = voxel->vertexIndices[lookupIndex];
          /* Find the two voxel cubes for the easy cases */
          for (int j = 0; j < 2; ++j) {
            switch (faces[j]) {
              case MC_CUBE_FACE_FRONT:
                voxel = &prevLine[x];
                break;
              case MC_CUBE_FACE_BOTTOM:
                voxel = &prevSlice[x + y * (x_res - 1)];
                break;
              case MC_CUBE_FACE_RIGHT:
                voxel = prevVoxel;
                break;
            }
            /* TODO: Use the mcNielsonDual_vertexIndexLookupTable to find the
             * vertex indices for each of the voxels on the interfacing faces, as
             * well as the voxel diagonal to this edge */
            /* NOTE: The flip edge of the given face/edge combination is
             * the given edge with respect to the cube on the other side of
             * the given face. */
            lookupIndex = mcNielsonDual_vertexIndexLookupTable[
              (mcCube_flipEdge(faces[j], edge) << 8) + voxel->cube];
            /* Find the vertex index for this voxel. This index must exist. */
            assert(voxel->vertices[lookupIndex] != -1);
            vertexIndices[j + 1] = voxel->vertices[lookupIndex];
          }
          /* Find the voxel cube diagonal to this edge */
          switch (edge) {
            case 0:
              /* The diagonal cube is on the bottom-front */
              if (y > 0) {
                voxel = &prevSlice[x + (y - 1) * (x_res - 1)];
              } else {
                 /* FIXME: Use special logic for cubes past the sample lattice */
                assert(false);
              }
              break;
            case 3:
              /* The diagonal cube is on the front-right */
              if (x > 0) {
                voxel = &prevLine[x - 1];
              } else {
                 /* FIXME: Use special logic for cubes past the sample lattice */
                assert(false);
              }
            case 8:
              if (x > 0) {
                /* The diagonal cube is on the bottom-right */
                cubes[2] = prevSlice[(x - 1) + y * (x_res - 1)];
              } else {
                 /* FIXME: Use special logic for cubes past the sample lattice */
                assert(false);
              }
          }
          lookupIndex = mcNielsonDual_vertexIndexLookupTable[
            (mc_cube_flipEdge(faces[1], mcCube_flipEdge(faces[0], edge)) << 8)
              + voxel->cube];
          /* Find the vertex index for this voxel. This index must exist. */
          assert(voxel->vertices[lookupIndex] != -1);
          vertexIndices[3] = voxel->vertices[lookupIndex];
          /* TODO: Note the signs of the samples on this edge to determine the
           * correct winding order. */
          /* FIXME: Enough information is available beforehand to determine the
           * winding order. We should be able to extract the winding order from
           * a table. */
        }
#endif
      }
    }
  }
  mcFace_destroy(&triangle);
}
