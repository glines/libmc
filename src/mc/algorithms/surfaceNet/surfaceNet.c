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

/* FIXME: Move this into mc/algorithms/common.h or something */
#include <mc/algorithms/simple/common.h>

#include <mc/algorithms/surfaceNet/surfaceNet.h>
#include <mc/vector.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

mcSurfaceNodePos mcSurfaceNodePos_opposite(mcSurfaceNodePos pos) {
  typedef struct PosPair {
    mcSurfaceNodePos pos[2];
  } PosPair;
  mcSurfaceNodePos table[] = {
    MC_SURFACE_NODE_BACK,  // FRONT
    MC_SURFACE_NODE_LEFT,  // RIGHT
    MC_SURFACE_NODE_BOTTOM,  // TOP
    MC_SURFACE_NODE_RIGHT,  // LEFT
    MC_SURFACE_NODE_TOP,  // BOTTOM
    MC_SURFACE_NODE_FRONT,  // BACK
  };
  assert(pos < 6);
  return table[pos];
}

void mcSurfaceNode_init(mcSurfaceNode *self) {
#ifndef NDEBUG
  /* Set all neighbor pointers to NULL */
  memset(self->neighbors, 0, sizeof(mcSurfaceNode*) * 6);
#endif
}

void mcSurfaceNode_destroy(mcSurfaceNode *self) {
}

void mcSurfaceNode_addNeighbor(
    mcSurfaceNode *self, mcSurfaceNode *neighbor, mcSurfaceNodePos pos)
{
  mcSurfaceNodePos opposite;
  assert(self->neighbors[pos] == NULL);
  self->neighbors[pos] = neighbor;
  opposite = mcSurfaceNodePos_opposite(pos);
  assert(neighbor->neighbors[opposite] == NULL);
  neighbor->neighbors[opposite] = NULL;
}

void mcSurfaceNode_setPosition(mcSurfaceNode *self, mcVec3 *pos) {
  /* Record the old position */
  self->oldPos = self->pos;
  /* Store the new position */
  self->pos = *pos;
}

/* TODO: Maybe MC_SURFACE_NET_NODES_PER_BLOCK should double with each block
 * allocated? That complicates things. */
const int MC_SURFACE_NET_NODES_PER_BLOCK = 1024;
const int MC_SURFACE_NET_INIT_POOL_SIZE = 16;

void mcSurfaceNet_init(mcSurfaceNet *self) {
  self->numNodes = 0;
  self->numBlocks = 0;
  self->nodePool = (mcSurfaceNode**)malloc(
      sizeof(mcSurfaceNode*) * MC_SURFACE_NET_INIT_POOL_SIZE);
  self->poolSize = MC_SURFACE_NET_INIT_POOL_SIZE;
}

void mcSurfaceNet_destroy(mcSurfaceNet *self) {
  for (int i = 0; i < self->numBlocks; ++i) {
    free(self->nodePool[i]);
  }
  free(self->nodePool);
}

void mcSurfaceNet_growBlockPool(mcSurfaceNet *self) {
  /* Double the size of the node pool */
  mcSurfaceNode **newPool = (mcSurfaceNode**)malloc(
      sizeof(mcSurfaceNode**) * self->poolSize * 2);
  memcpy(newPool, self->nodePool, sizeof(mcSurfaceNode**) * self->poolSize);
  free(self->nodePool);
  self->nodePool = newPool;
  self->poolSize *= 2;
}

void mcSurfaceNet_addNodeBlock(mcSurfaceNet *self) {
  /* Make sure we can reference as many blocks */
  if (self->numBlocks >= self->poolSize) {
    mcSurfaceNet_growBlockPool(self);
  }
  self->nodePool[self->numBlocks++] = (mcSurfaceNode*)malloc(
        sizeof(mcSurfaceNode) * MC_SURFACE_NET_NODES_PER_BLOCK);
}

mcSurfaceNode *mcSurfaceNet_getNode(mcSurfaceNet *self, unsigned int i) {
  assert(i < self->numNodes);
  assert(i / MC_SURFACE_NET_NODES_PER_BLOCK < self->numBlocks);
  return self->nodePool[i / MC_SURFACE_NET_NODES_PER_BLOCK]
          + i % MC_SURFACE_NET_NODES_PER_BLOCK;
}

mcSurfaceNode *mcSurfaceNet_getNextNode(mcSurfaceNet *self) {
  mcSurfaceNode *nextNode;
  /* Make sure we have a block allocated for this node */
  if (self->numNodes / MC_SURFACE_NET_NODES_PER_BLOCK >= self->numBlocks) {
    mcSurfaceNet_addNodeBlock(self);
  }
  /* Get the address for the next node in the correct block */
  nextNode = self->nodePool[self->numNodes / MC_SURFACE_NET_NODES_PER_BLOCK]
              + self->numNodes % MC_SURFACE_NET_NODES_PER_BLOCK;
  mcSurfaceNode_init(nextNode);
  self->numNodes += 1;
  return nextNode;
}

void mcSurfaceNet_isosurfaceFromField(
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int res_x, unsigned int res_y, unsigned int res_z,
    const mcVec3 *min, const mcVec3 *max,
    mcMesh *mesh)
{
  mcSurfaceNet surfaceNet;
  float delta_x = fabs(max->x - min->x) / (float)res_x;
  float delta_y = fabs(max->y - min->y) / (float)res_y;
  float delta_z = fabs(max->z - min->z) / (float)res_z;
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
          mcSimpleVertexRelativePosition(vertex, pos);
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
          mcSurfaceNode *node, *backNeighbor, *bottomNeighbor, *leftNeighbor;
          /* Create a node for this surface cube */
          node = mcSurfaceNet_getNextNode(&surfaceNet);
          /* Set the node position to the center of the voxel cube */
          node->pos.x = min->x + x * delta_x + delta_x / 2;
          node->pos.y = min->y + y * delta_y + delta_y / 2;
          node->pos.z = min->z + z * delta_z + delta_z / 2;
          /* There is no old position, so we store it just the same */
          node->oldPos = node->pos;
          /* Store the lattice position of the node */
          node->latticePos[0] = x;
          node->latticePos[1] = y;
          node->latticePos[2] = z;
          /* Attach surface node to neighboring surface nodes */
          backNeighbor = prevSlice[y * (res_x - 1) + x];
          if (backNeighbor != NULL) {
            mcSurfaceNode_addNeighbor(
                node, backNeighbor, MC_SURFACE_NODE_BACK);
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
  static const int MAX_ITERATIONS = 10;
  for (int i = 0; i < MAX_ITERATIONS; ++i) {
    /* Iterate over the surface net */
    for (int j = 0; j < surfaceNet.numNodes; ++j) {
      mcSurfaceNode *node;
      mcVec3 midPoint;
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
        /* Restrict the new position to within the voxel cube */
        midPoint.x = max(midPoint.x,
            (float)node->latticePos[0] * delta_x);
        midPoint.x = min(midPoint.x,
            (float)(node->latticePos[0] + 1) * delta_x);
        midPoint.y = max(midPoint.y,
            (float)node->latticePos[1] * delta_y);
        midPoint.y = min(midPoint.y,
            (float)(node->latticePos[1] + 1) * delta_y);
        midPoint.z = max(midPoint.z,
            (float)node->latticePos[2] * delta_z);
        midPoint.z = min(midPoint.z,
            (float)(node->latticePos[2] + 1) * delta_z);
        /* Set the new surface node position */
        mcSurfaceNode_setPosition(node, &midPoint);
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
  }
  /* Free node buffers */
  free(prevLine);
  free(prevSlice);
  /* Destroy the surface net, freeing all memory */
  mcSurfaceNet_destroy(&surfaceNet);
  /* Destroy the triangle */
  mcFace_destroy(&triangle);
}
