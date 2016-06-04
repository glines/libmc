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
#include <stdlib.h>
#include <string.h>

#include <mc/algorithms/common/surfaceNet.h>

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
