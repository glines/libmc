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

#include <mc/algorithms/common/surfaceNet.h>

mcSurfaceNodePos mcSurfaceNodePos_opposite(mcSurfaceNodePos pos) {
  typedef struct PosPair {
    mcSurfaceNodePos pos[2];
  } PosPair;
  mcSurfaceNodePos table[] = {
    MC_SURFACE_NODE_BACK,  // FRONT
    MC_SURFACE_NODE_RIGHT,  // LEFT
    MC_SURFACE_NODE_BOTTOM,  // TOP
    MC_SURFACE_NODE_TOP,  // BOTTOM
    MC_SURFACE_NODE_LEFT,  // RIGHT
    MC_SURFACE_NODE_FRONT,  // BACK
  };
  assert(pos < 6);
  return table[pos];
}

void mcSurfaceNode_init(mcSurfaceNode *self) {
  /* Set all neighbor pointers to NULL */
  memset(self->neighbors, 0, sizeof(mcSurfaceNode*) * 6);
}

void mcSurfaceNode_destroy(mcSurfaceNode *self) {
  /* TODO: Inform neighbors that our node is destroyed? Might harm performance
   * of surface net destruction. */
}

void mcSurfaceNode_addNeighbor(
    mcSurfaceNode *self, mcSurfaceNode *neighbor, mcSurfaceNodePos pos)
{
  mcSurfaceNodePos opposite;
  assert(self->neighbors[pos] == NULL);
  self->neighbors[pos] = neighbor;
  opposite = mcSurfaceNodePos_opposite(pos);
  assert(neighbor->neighbors[opposite] == NULL);
  neighbor->neighbors[opposite] = self;
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

void mcSurfaceNet_build(mcSurfaceNet *self,
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int res_x, unsigned int res_y, unsigned int res_z,
    const mcVec3 *min, const mcVec3 *max)
{
  float delta_x = fabs(max->x - min->x) / (float)(res_x - 1);
  float delta_y = fabs(max->y - min->y) / (float)(res_y - 1);
  float delta_z = fabs(max->z - min->z) / (float)(res_z - 1);

  /* Keep a slice, line, and voxel buffer of surface nodes so that we can
   * find nodes relative to the current node. */
  mcSurfaceNode **prevSlice =
    (mcSurfaceNode**)malloc(sizeof(mcSurfaceNode*) * (res_x - 1) * (res_y - 1));
  /* The start of the algorithm has no previous slice */
  memset(prevSlice, 0, sizeof(mcSurfaceNode*) * (res_x - 1) * (res_y - 1));
  mcSurfaceNode **prevLine = 
    (mcSurfaceNode**)malloc(sizeof(mcSurfaceNode*) * (res_x - 1));
  mcSurfaceNode *prevVoxel;
  /* We start by generating the surface net */
  /* Iterate over the cube lattice (the dual of the sample lattice) */
  for (unsigned int z = 0; z < res_z - 1; ++z) {
    /* The start of a new slice has no previous line */
    memset(prevLine, 0, sizeof(mcSurfaceNode*) * (res_x - 1));
    for (unsigned int y = 0; y < res_y - 1; ++y) {
      /* The start of a new line has no previous voxel */
      prevVoxel = NULL;
      for (unsigned int x = 0; x < res_x - 1; ++x) {
        unsigned int cube;
        float samples[8];
        /* Gather a sample from each of the cube's eight vertices */
        /* TODO: Many of these samples can be stored/retrieved from a cache */
        for (unsigned int sampleIndex = 0; sampleIndex < 8; ++sampleIndex) {
          /* Determine this sample's relative position in the cube */
          unsigned int pos[3];
          mcCube_sampleRelativePosition(sampleIndex, pos);
          samples[sampleIndex] = sf(
              min->x + (x + pos[0]) * delta_x,
              min->y + (y + pos[1]) * delta_y,
              min->z + (z + pos[2]) * delta_z,
              args);
        }
        /* Determine the cube configuration from our samples */
        cube = mcCube_cubeConfigurationFromSamples(samples);
        if (cube != 0x00 && cube != 0xff) {
          /* Add cubes that intersect the surface to the surface net */
          mcSurfaceNode *node, *bottomNeighbor, *frontNeighbor, *rightNeighbor;
          /* Create a surface node for this surface cube */
          node = mcSurfaceNet_getNextNode(self);
          /* Set the node position to the center of the voxel cube */
          node->pos.x = min->x + x * delta_x + delta_x / 2.0f;
          node->pos.y = min->y + y * delta_y + delta_y / 2.0f;
          node->pos.z = min->z + z * delta_z + delta_z / 2.0f;
          /* Store the lattice position of the node */
          node->latticePos[0] = x;
          node->latticePos[1] = y;
          node->latticePos[2] = z;
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
          rightNeighbor = prevVoxel;
          if (rightNeighbor != NULL) {
            mcSurfaceNode_addNeighbor(
                node, rightNeighbor, MC_SURFACE_NODE_RIGHT);
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
  /* Free our allocated resources */
  free(prevLine);
  free(prevSlice);
}

void mcSurfaceNet_updateOldPos(mcSurfaceNet *self) {
  /* Iterate through each node and set the old node position to the node's
   * current position. */
  for (int i = 0; i < self->numNodes; ++i) {
    mcSurfaceNode *node;
    node = mcSurfaceNet_getNode(self, i);
    node->oldPos = node->pos;
  }
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
