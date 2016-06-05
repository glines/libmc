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
#include <stdlib.h>
#include <string.h>

#include <stdio.h> /* XXX */

#include <mc/algorithms/common/cube.h>
#include <mc/algorithms/common/surfaceNet.h>
#include <mc/algorithms/cuberille/cuberille.h>
#include <mc/mesh.h>

/**
 * Implementation of a simple "cuberille" isosurface extraction algorithm as
 * described by <https://0fps.net/2012/07/12/smooth-voxel-terrain-part-2/>.
 * This algorithm is the precursor to elastic surface nets and other "dual"
 * methods.
 */
void mcCuberille_isosurfaceFromField(
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int res_x, unsigned int res_y, unsigned int res_z,
    const mcVec3 *min, const mcVec3 *max,
    mcMesh *mesh)
{
  mcSurfaceNet surfaceNet;
  mcFace triangle;
  float delta_x = fabs(max->x - min->x) / (float)(res_x - 1);
  float delta_y = fabs(max->y - min->y) / (float)(res_y - 1);
  float delta_z = fabs(max->z - min->z) / (float)(res_z - 1);
  /* Initialize the surface net structure */
  mcSurfaceNet_init(&surfaceNet);
  /* Keep a slice, line, and voxel buffer of surface nodes so that we can
   * find nodes relative to the current node. */
  mcSurfaceNode **prevSlice =
    (mcSurfaceNode**)malloc(sizeof(mcSurfaceNode*) * (res_x - 1) * (res_y - 1));
  memset(prevSlice, 0, sizeof(mcSurfaceNode*) * (res_x - 1) * (res_y - 1));
  mcSurfaceNode **prevLine = 
    (mcSurfaceNode**)malloc(sizeof(mcSurfaceNode*) * (res_x - 1));
  mcSurfaceNode *prevVoxel;
  /* TODO: We start by generating the surface net */
  /* Iterate over the sample lattice */
  for (unsigned int z = 0; z < res_z - 1; ++z) {
    /* The start of a new slice has no previous line */
    memset(prevLine, 0, sizeof(mcSurfaceNode*) * (res_x - 1));
    for (unsigned int y = 0; y < res_y - 1; ++y) {
      /* The start of a new line has no previous voxel */
      prevVoxel = NULL;
      for (unsigned int x = 0; x < res_x - 1; ++x) {
        unsigned int cube;
        float samples[8];
        /* TODO: Gather a sample from each of the cube's eight vertices */
        /* TODO: Many of these samples can be stored/retrieved from a cache */
        for (unsigned int vertex = 0; vertex < 8; ++vertex) {
          /* Determine this vertex's relative position in the cube */
          unsigned int pos[3];
          mcCube_vertexRelativePosition(vertex, pos);
          samples[vertex] = sf(
              min->x + (x + pos[0]) * delta_x,
              min->y + (y + pos[1]) * delta_y,
              min->z + (z + pos[2]) * delta_z,
              args);
          fprintf(stderr, "samples[%d]: %g\n",
              vertex, samples[vertex]);  /* XXX */
        }
        /* Determine the cube configuration from our samples */
        cube = mcCube_cubeConfigurationFromSamples(samples);
        fprintf(stderr, "cube: 0x%02x\n", cube);  /* XXX */
        /* TODO: Add cubes that intersect the surface to the surface net */
        if (cube != 0x00 && cube != 0xff) {
          mcSurfaceNode *node, *bottomNeighbor, *frontNeighbor, *leftNeighbor;
          /* Create a surface node for this surface cube */
          node = mcSurfaceNet_getNextNode(&surfaceNet);
          /* Set the node position to the center of the voxel cube */
          node->pos.x = min->x + x * delta_x + delta_x / 2.0f;
          node->pos.y = min->y + y * delta_y + delta_y / 2.0f;
          node->pos.z = min->z + z * delta_z + delta_z / 2.0f;
          /* Connect surface node to neighboring surface nodes */
          bottomNeighbor = prevSlice[y * (res_x - 1) + x];
          if (bottomNeighbor != NULL) {
            mcSurfaceNode_addNeighbor(
                node, bottomNeighbor, MC_SURFACE_NODE_BOTTOM);
          }
          frontNeighbor = prevLine[x];
          if (frontNeighbor != NULL) {
            mcSurfaceNode_addNeighbor(
                node, frontNeighbor, MC_SURFACE_NODE_FRONT);
          }
          leftNeighbor = prevVoxel;
          if (leftNeighbor != NULL) {
            mcSurfaceNode_addNeighbor(
                node, leftNeighbor, MC_SURFACE_NODE_LEFT);
          }
          /* Record this surface node in the prev node buffers */
          prevSlice[y * (res_x - 1) + x] = node;
          prevLine[x] = node;
          prevVoxel = node;
        } else {
          /* Record the absence of a surface node node as NULL in the prev node
           * buffers */
          prevSlice[y * (res_x - 1) + x] = NULL;
          prevLine[x] = NULL;
          prevVoxel = NULL;
        }
      }
    }
  }
  fprintf(stderr, "numNodes: %d\n", surfaceNet.numNodes);
  /* Now that we have the surface net, we generate a vertex for each surface
   * node */
  mcFace_init(&triangle, 3);
  for (int i = 0; i < surfaceNet.numNodes; ++i) {
    mcSurfaceNode *node;
    mcVertex vertex;
    node = mcSurfaceNet_getNode(&surfaceNet, i);
    vertex.pos = node->pos;
    /* TODO: Calculate the vertex normal */
    /* Add the vertex for this node to the mesh */
    node->vertexIndex = mcMesh_addVertex(mesh, &vertex);
    /* XXX: We generate some fake triangles so we can visualize where the
     * surface nodes have been placed */
    vertex.pos.x += delta_x / 64.0f;
    mcMesh_addVertex(mesh, &vertex);
    vertex.pos.y += delta_y / 64.0f;
    mcMesh_addVertex(mesh, &vertex);
    triangle.indices[0] = node->vertexIndex;
    triangle.indices[1] = node->vertexIndex + 1;
    triangle.indices[2] = node->vertexIndex + 2;
    mcMesh_addFace(mesh, &triangle);
  }
  /* With the vertex indices generated, we can now generate triangles */
  for (int i = 0; i < surfaceNet.numNodes; ++i) {
    mcSurfaceNode *node;
    node = mcSurfaceNet_getNode(&surfaceNet, i);
    /* XXX: Draw a thin triangle to each of our neighbor nodes (poor man's
     *      line) */
    for (int j = 0; j < MC_CUBE_NUM_FACES; ++j) {
      mcVertex vertex;
      unsigned int vertexIndex;
      if (node->neighbors[j] == NULL)
        continue;
      vertex.pos = node->neighbors[j]->pos;
      vertex.pos.x += delta_x / 64.0f;
      vertex.pos.y += delta_y / 64.0f;
      vertex.pos.z += delta_z / 64.0f;
      vertexIndex = mcMesh_addVertex(mesh, &vertex);
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = node->neighbors[j]->vertexIndex;
      triangle.indices[2] = vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
  }
  /* Free our resources */
  mcFace_destroy(&triangle);
  mcSurfaceNet_destroy(&surfaceNet);
  free(prevLine);
  free(prevSlice);
}
