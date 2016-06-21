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

/**
 * \addtogroup libmc
 * @{
 */

/**
 * \addtogroup algorithms
 * @{
 */

/**
 * \addtogroup common
 * @{
 */

/**
 * \defgroup mcSurfaceNet mcSurfaceNet
 */

/**
 * \addtogroup mcSurfaceNet
 * @{
 */

#include <mc/vector.h>
#include <mc/isosurfaceBuilder.h>  /* FIXME: This should be mc/scalarField.h */

/**
 * This enum encodes the spatial neighbor relatinoship between cube voxels in
 * the surface net.
 *
 * \todo The mcSurfaceNodePos enum should be replaced with an enum from
 * mc/algorithms/common/cubes.h.
 */
typedef enum mcSurfaceNodePos {
  MC_SURFACE_NODE_FRONT = 0,
  MC_SURFACE_NODE_LEFT,
  MC_SURFACE_NODE_TOP,
  MC_SURFACE_NODE_BOTTOM,
  MC_SURFACE_NODE_RIGHT,
  MC_SURFACE_NODE_BACK,
} mcSurfaceNodePos;

/**
 * This routine returns the opposite face on the cube for a voxel cube neighbor
 * relationship in the surface net.
 *
 * \param pos The surface node neighbor position of which to find the opposite
 * position.
 * \return The opposite neighbor position of the given position.
 *
 * \todo This routine can be replaced with a routine from
 * mc/algorithms/common/cubes.h.
 */
mcSurfaceNodePos mcSurfaceNodePos_opposite(mcSurfaceNodePos pos);

typedef struct mcSurfaceNode mcSurfaceNode;
/**
 * Struct representing a single node in a surface net. This struct stores all
 * of the information needed to associate a surface node with its neighbors and
 * adjust its position accordingly.
 */
struct mcSurfaceNode {
  mcSurfaceNode *neighbors[6];
  mcVec3 pos, oldPos;
  unsigned int latticePos[3];
  unsigned int vertexIndex;
};

/**
 * Initilaizes the given surface node. Surface nodes do not use any dynamically
 * allocated memory, so this routine simply initializes the node neighbor
 * pointers to NULL.
 *
 * \param self The surface node structure to initialize.
 */
void mcSurfaceNode_init(mcSurfaceNode *self);

/**
 * Destroys the given surface node.
 *
 * \param self The surface node structure to destroy.
 *
 * \todo It might be nice if the surface node destructor cleared up any
 * pointers that neighbors have to this node. This behavior is not yet needed
 * at this time, and might harm performance when the surface net itself is
 * destroyed.
 */
void mcSurfaceNode_destroy(mcSurfaceNode *self);

/**
 * Adds a neighbor in the given direction to the given surface node.
 */
void mcSurfaceNode_addNeighbor(
    mcSurfaceNode *self, mcSurfaceNode *neighbor, mcSurfaceNodePos pos);

/**
 * Sets the vertex position of the given surface node.
 */
void mcSurfaceNode_setPosition(mcSurfaceNode *self, mcVec3 *pos);

/** The number of surface nodes that are allocated per node block in the
 * mcSurfaceNet structure. */
extern const int MC_SURFACE_NET_NODES_PER_BLOCK;
/** The initial size of the node pool in the mcSurfaceNet structure. */
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
  /** Pool of pointers to node blocks in the surface net. This pool is expected
   * to grow as the surface net is built. The memory pointed to by these
   * pointers is never re-allocated, since the structure of the surface net
   * depends on these memory addresses remaining valid. */
  mcSurfaceNode **nodePool;
  /** The number of pointers that the node pool currently has allocated. */
  unsigned int poolSize;
  /** The number of blocks that are currently allocated and for which we have
   * pointers to in the node pool. */
  unsigned int numBlocks;
  /** The actual number of nodes that have been added to the surface net. */
  unsigned int numNodes;
} mcSurfaceNet;

/**
 * Initializes the given surface net. The surface net structure is designed to
 * grow as surface nodes are added, so it is important that these dynamic data
 * structures are allocated.
 *
 * \param self The surface net structure to initialize.
 */
void mcSurfaceNet_init(mcSurfaceNet *self);

/**
 * Destroys the given surface net. This frees all memory associated with the
 * surface net and its nodes.
 *
 * \param self The surface net structure to destroy.
 */
void mcSurfaceNet_destroy(mcSurfaceNet *self);

/**
 * Builds a surface net for the isosurface defined by the given scalar field.
 * This routine is essentially a simple implementation of the cuberille (e.g.
 * Minecraft) isosurface extraction algorithm. The vertex positions of the
 * nodes in the resulting surface net can be further refined by whatever
 * surface net algorithm is being implemented.
 *
 * \param self The surface net on which to build the isosurface representation.
 * \param sf The scalar field defining the underlying isosurface for which we
 * are constructing a surface net.
 * \param args Auxiliary arguments for the scalar field function, for
 * implementing "functor" scalar fields and other flexible implementations.
 * \param res_x The number of samples to take in the sample lattice parallel to
 * the x-axis.
 * \param res_y The number of samples to take in the sample lattice parallel to
 * the y-axis.
 * \param res_z The number of samples to take in the sample lattice parallel to
 * the z-axis.
 * \param min The absolute position where the sample lattice begins and the
 * first sample is to be taken.
 * \param max The absolute position where the sample lattice ends and the last
 * sample is to be taken.
 *
 * Note that the call signature of this method reflects those of
 * mcIsosurfaceBuilder.
 */
void mcSurfaceNet_build(mcSurfaceNet *self,
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int res_x, unsigned int res_y, unsigned int res_z,
    const mcVec3 *min, const mcVec3 *max);

/**
 * Records the current position of the surface nodes as their old positions.
 * This method facilitates writing surface net algorithms that iteratively
 * improve the vertex positions of surface nodes.
 *
 * \param self The surface net structure whose old surface node positions are
 * to be updated.
 */
void mcSurfaceNet_updateOldPos(mcSurfaceNet *self);

/**
 * Doubles the number of node block pointers that can be referred to by this
 * surface net.
 *
 * \param self The surface net structure whose node block pool data structure
 * is to be doubled.
 */
void mcSurfaceNet_growBlockPool(mcSurfaceNet *self);

/**
 * Allocates a block of memory for storing more surface net nodes in this
 * surface net.
 *
 * \param self The surface net structure to which we are adding a node block.
 */
void mcSurfaceNet_addNodeBlock(mcSurfaceNet *self);

/**
 * Returns a pointer to the surface node at the given surface node index.
 *
 * \param self The surface node structure whose surface node we are retrieving.
 * \param i The index of the surface node we are accessing.
 * \return A pointer to the surface node at index \p i.
 */
mcSurfaceNode *mcSurfaceNet_getNode(mcSurfaceNet *self, unsigned int i);

/**
 * Adds a surface node to the surface net and returns a pointer to the memory
 * for that new surface node.
 *
 * \param self The surface node structure to which we are adding a new surface
 * node.
 * \return A pointer to the newly added surface node.
 */
mcSurfaceNode *mcSurfaceNet_getNextNode(mcSurfaceNet *self);

/** @} */

/** @} */

/** @} */

/** @} */

#endif
