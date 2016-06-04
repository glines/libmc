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

#include <stdio.h>  /* XXX */

#include <mc/algorithms/common/cube.h>

#include <mc/algorithms/common/surfaceNet.h>
#include <mc/algorithms/surfaceNet/surfaceNet.h>
#include <mc/vector.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define squared(a) ((a) * (a))

void mcSurfaceNet_isosurfaceFromField(
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int res_x, unsigned int res_y, unsigned int res_z,
    const mcVec3 *min, const mcVec3 *max,
    mcMesh *mesh)
{
  mcSurfaceNet surfaceNet;
  float delta_x = fabs(max->x - min->x) / (float)(res_x - 1);
  float delta_y = fabs(max->y - min->y) / (float)(res_y - 1);
  float delta_z = fabs(max->z - min->z) / (float)(res_z - 1);
  mcFace triangle;
  /* Initialize the surface net structure */
  mcSurfaceNet_init(&surfaceNet);
  /* Keep a slice, line, and voxel buffer of surface nodes so that we can
   * find nodes relative to the current node. */
  mcSurfaceNode **prevSlice =
    (mcSurfaceNode**)malloc(sizeof(mcSurfaceNode*) * (res_x - 1) * (res_y - 1));
  memset(prevSlice, 0, sizeof(mcSurfaceNode*) * (res_x - 1) * (res_y - 1));
  mcSurfaceNode **prevLine = 
    (mcSurfaceNode**)malloc(sizeof(mcSurfaceNode*) * (res_x - 1));
  memset(prevLine, 0, sizeof(mcSurfaceNode*) * (res_x - 1));
  mcSurfaceNode *prevVoxel = 0;
  /* Iterate over the voxel cube lattice */
  for (unsigned int z = 0; z < res_z - 1; ++z) {
    for (unsigned int y = 0; y < res_y - 1; ++y) {
      for (unsigned int x = 0; x < res_x - 1; ++x) {
        /* Determine the cube configuration index by iterating over the eight
         * cube vertices.
         * NOTE: Copied from mc/algorithms/simple/simple.c
         */
        unsigned int cube = 0;
        for (unsigned int vertex = 0; vertex < 8; ++vertex) {
          /* Determine this vertex's relative position in the cube */
          unsigned int pos[3];
          float sample;
          mcCube_vertexRelativePosition(vertex, pos);
          /* TODO: Many of these sample values can be stored/retrieved from a
           * cache */
          sample = sf(
              min->x + (x + pos[0]) * delta_x,
              min->y + (y + pos[1]) * delta_y,
              min->z + (z + pos[2]) * delta_z,
              args);
          fprintf(stderr, "sample: %g\n", sample);  /* XXX */
          /* Add the bit this vertex contributes to the cube */
          cube |= (sample >= 0.0f ? 1 : 0) << vertex;
        }
        fprintf(stderr, "cube: 0x%02x\n", cube);  /* XXX */
        assert(cube <= 0xff);
        /* Check for surface cubes, i.e. cubes that intersect the isosurface */
        if (cube != 0x00 && cube != 0xff) {
          mcSurfaceNode *node, *frontNeighbor, *bottomNeighbor, *leftNeighbor;
          /* Create a node for this surface cube */
          node = mcSurfaceNet_getNextNode(&surfaceNet);
          /* Set the node position to the center of the voxel cube */
          node->pos.x = min->x + x * delta_x + delta_x / 2.0f;
          node->pos.y = min->y + y * delta_y + delta_y / 2.0f;
          node->pos.z = min->z + z * delta_z + delta_z / 2.0f;
          /* Store the lattice position of the node */
          node->latticePos[0] = x;
          node->latticePos[1] = y;
          node->latticePos[2] = z;
          /* Connect surface node to neighboring surface nodes */
          frontNeighbor = prevSlice[y * (res_x - 1) + x];
          if (frontNeighbor != NULL) {
            mcSurfaceNode_addNeighbor(
                node, frontNeighbor, MC_SURFACE_NODE_FRONT);
          }
          bottomNeighbor = prevLine[x];
          if (bottomNeighbor != NULL) {
            mcSurfaceNode_addNeighbor(
                node, bottomNeighbor, MC_SURFACE_NODE_BOTTOM);
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
  /* TODO: Now that the surface net has been built, we iterate to improve it */
  /* TODO: Allow the number of iterations to be passed as an argument */
  static const int MAX_ITERATIONS = 300;
  for (int i = 0; i < MAX_ITERATIONS; ++i) {
    fprintf(stderr, "iteration: %d\n", i);
    /* Update the old position of all nodes */
    for (int j = 0; j < surfaceNet.numNodes; ++j) {
      mcSurfaceNode *node;
      node = mcSurfaceNet_getNode(&surfaceNet, j);
      node->oldPos = node->pos;
    }
    /* Iterate over the surface net */
    for (int j = 0; j < surfaceNet.numNodes; ++j) {
      mcSurfaceNode *node;
      mcVec3 midPoint, newPos;
      int numNeighbors;
      node = mcSurfaceNet_getNode(&surfaceNet, j);
      /* TODO: Relax the position of surface nodes to reduce energy between
       * neighboring nodes */
      /* Compute a point equidistant among neighbor positions */
      numNeighbors = 0;
      for (int k = 0; k < 6; ++k) {
        if (node->neighbors[k] == NULL)
          continue;
        numNeighbors += 1;
        midPoint.x += node->neighbors[k]->oldPos.x;
        midPoint.y += node->neighbors[k]->oldPos.y;
        midPoint.z += node->neighbors[k]->oldPos.z;
      }
      if (numNeighbors > 0) {
        midPoint.x /= (float)numNeighbors;
        midPoint.y /= (float)numNeighbors;
        midPoint.z /= (float)numNeighbors;
        /* TODO: Weight the midpoint into the new position */
        const float WEIGHT = 1.0f;
        newPos.x = (1.0f - WEIGHT) * node->pos.x + WEIGHT * midPoint.x;
        newPos.y = (1.0f - WEIGHT) * node->pos.y + WEIGHT * midPoint.y;
        newPos.z = (1.0f - WEIGHT) * node->pos.z + WEIGHT * midPoint.z;
        /* Restrict the new position to within the voxel cube */
        newPos.x = max(newPos.x,
            min->x + (float)node->latticePos[0] * delta_x);
        newPos.x = min(newPos.x,
            min->x + (float)(node->latticePos[0] + 1) * delta_x);
        newPos.y = max(newPos.y,
            min->y + (float)node->latticePos[1] * delta_y);
        newPos.y = min(newPos.y,
            min->y + (float)(node->latticePos[1] + 1) * delta_y);
        newPos.z = max(newPos.z,
            min->z + (float)node->latticePos[2] * delta_z);
        newPos.z = min(newPos.z,
            min->z + (float)(node->latticePos[2] + 1) * delta_z);
        /* Compute old and new energy as sum of squared distance to neighbors */
        float oldEnergy, newEnergy;
        oldEnergy = newEnergy = 0.0f;
        for (int k = 0; k < 6; ++k) {
          if (node->neighbors[k] == NULL)
            continue;
          oldEnergy += squared(node->neighbors[k]->oldPos.x - node->oldPos.x)
            + squared(node->neighbors[k]->oldPos.y - node->oldPos.y)
            + squared(node->neighbors[k]->oldPos.z - node->oldPos.z);
          newEnergy += squared(node->neighbors[k]->oldPos.x - newPos.x)
            + squared(node->neighbors[k]->oldPos.y - newPos.y)
            + squared(node->neighbors[k]->oldPos.z - newPos.z);
        }
        fprintf(stderr, "oldEnergy: %g, newEnergy: %g\n",
            oldEnergy, newEnergy);
        if (newEnergy < oldEnergy) {
          /* Set the new surface node position */
          node->pos = newPos;
        }
      }
    }
  }
  /* Now that the surface nodes are in their final positions, we generate
   * vertices */
  /* Iterate over the surface net */
  for (int i = 0; i < surfaceNet.numNodes; ++i) {
    mcSurfaceNode *node;
    mcVertex vertex;
    node = mcSurfaceNet_getNode(&surfaceNet, i);
    vertex.pos = node->pos;
    /* TODO: Calculate the vertex normal */
    /* Add the vertex for this node to the mesh */
    node->vertexIndex = mcMesh_addVertex(mesh, &vertex);
  }
  /* TODO: With the surface net optimized, we now generate triangles from it */
  /* TODO: Initialize the placeholder triangle */
  mcFace_init(&triangle, 3);
  /* TODO: Iterate over the surface net */
  for (int i = 0; i < surfaceNet.numNodes; ++i) {
    mcSurfaceNode *node, *leftNeighbor, *rightNeighbor, *topNeighbor,
                  *bottomNeighbor, *frontNeighbor, *backNeighbor;
    node = mcSurfaceNet_getNode(&surfaceNet, i);
    leftNeighbor = node->neighbors[MC_SURFACE_NODE_LEFT];
    rightNeighbor = node->neighbors[MC_SURFACE_NODE_RIGHT];
    topNeighbor = node->neighbors[MC_SURFACE_NODE_TOP];
    bottomNeighbor = node->neighbors[MC_SURFACE_NODE_BOTTOM];
    frontNeighbor = node->neighbors[MC_SURFACE_NODE_FRONT];
    backNeighbor = node->neighbors[MC_SURFACE_NODE_BACK];
    /* Generate triangles, taking care not to generate any redundant
     * triangles */
    /* FIXME: Mind the winding order of triangle */
    /* TODO: Support inverted pairs of triangles with a flag? As of now, we
     * choose arbitrarily which triangle pairs to generate. */
    /* Look for pairs of neighbors with which we can create triangles */
    /* LEFT+FRONT and BACK+RIGHT or LEFT+BACK and FRONT+RIGHT */
    if (leftNeighbor && frontNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = leftNeighbor->vertexIndex;
      triangle.indices[2] = frontNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    if (backNeighbor && rightNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = backNeighbor->vertexIndex;
      triangle.indices[2] = rightNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    /*
    if (leftNeighbor && backNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = leftNeighbor->vertexIndex;
      triangle.indices[2] = backNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    if (frontNeighbor && rightNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = frontNeighbor->vertexIndex;
      triangle.indices[2] = rightNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    */
    /* FRONT+TOP and BOTTOM+BACK or FRONT+BOTTOM and TOP+BACK */
    if (frontNeighbor && topNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = frontNeighbor->vertexIndex;
      triangle.indices[2] = topNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    if (bottomNeighbor && backNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = bottomNeighbor->vertexIndex;
      triangle.indices[2] = backNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    /*
    if (frontNeighbor && bottomNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = frontNeighbor->vertexIndex;
      triangle.indices[2] = bottomNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    if (topNeighbor && backNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = topNeighbor->vertexIndex;
      triangle.indices[2] = backNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    */
    /* LEFT+TOP and BOTTOM+RIGHT or LEFT+BOTTOM and TOP+RIGHT */
    if (leftNeighbor && topNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = leftNeighbor->vertexIndex;
      triangle.indices[2] = topNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    if (bottomNeighbor && rightNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = bottomNeighbor->vertexIndex;
      triangle.indices[2] = rightNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    /*
    if (leftNeighbor && bottomNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = leftNeighbor->vertexIndex;
      triangle.indices[2] = bottomNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    if (topNeighbor && rightNeighbor) {
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = topNeighbor->vertexIndex;
      triangle.indices[2] = rightNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    */
  }
  /* Free node buffers */
  free(prevLine);
  free(prevSlice);
  /* Destroy the surface net, freeing all memory */
  mcSurfaceNet_destroy(&surfaceNet);
  /* Destroy the triangle */
  mcFace_destroy(&triangle);
}
