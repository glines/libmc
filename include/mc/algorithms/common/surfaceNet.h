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

#ifndef MC_ALGORITHMS_COMMON_SURFACE_NET_H_
#define MC_ALGORITHMS_COMMON_SURFACE_NET_H_

#include <mc/vector.h>

typedef enum mcSurfaceNodePos {
  MC_SURFACE_NODE_FRONT = 0,
  MC_SURFACE_NODE_LEFT,
  MC_SURFACE_NODE_TOP,
  MC_SURFACE_NODE_BOTTOM,
  MC_SURFACE_NODE_RIGHT,
  MC_SURFACE_NODE_BACK,
} mcSurfaceNodePos;

mcSurfaceNodePos mcSurfaceNodePos_opposite(mcSurfaceNodePos pos);

typedef struct mcSurfaceNode mcSurfaceNode;
struct mcSurfaceNode {
  mcSurfaceNode *neighbors[6];
  mcVec3 pos, oldPos;
  unsigned int latticePos[3];
  unsigned int vertexIndex;
};

void mcSurfaceNode_init(mcSurfaceNode *self);

void mcSurfaceNode_destroy(mcSurfaceNode *self);

void mcSurfaceNode_addNeighbor(
    mcSurfaceNode *self, mcSurfaceNode *neighbor, mcSurfaceNodePos pos);

void mcSurfaceNode_setPosition(mcSurfaceNode *self, mcVec3 *pos);

extern const int MC_SURFACE_NET_NODES_PER_BLOCK;
extern const int MC_SURFACE_NET_INIT_POOL_SIZE;

/**
 * The surface net is structured as a pool of surface nodes which are allocated
 * as needed.
 *
 * The surface nodes themselves use pointers to represent the graph, making it
 * prohibitively expensive to move surface nodes around in memory.
 * mcSurfaceNet is structured to avoid moving surface nodes as the number of
 * nodes increases.
 */
typedef struct mcSurfaceNet {
  mcSurfaceNode **nodePool;
  unsigned int poolSize;
  unsigned int numBlocks;
  unsigned int numNodes;
} mcSurfaceNet;

void mcSurfaceNet_init(mcSurfaceNet *self);

void mcSurfaceNet_destroy(mcSurfaceNet *self);

void mcSurfaceNet_growBlockPool(mcSurfaceNet *self);

void mcSurfaceNet_addNodeBlock(mcSurfaceNet *self);

mcSurfaceNode *mcSurfaceNet_getNode(mcSurfaceNet *self, unsigned int i);

mcSurfaceNode *mcSurfaceNet_getNextNode(mcSurfaceNet *self);

#endif
