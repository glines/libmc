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
#include <stddef.h>

#include <mc/algorithms/common/cube.h>

#include <mc/algorithms/common/surfaceNet.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define squared(a) ((a) * (a))

float mcElasticSurfaceNet_nodeEnergy(const mcSurfaceNode *node) {
  float energy;
  /* TODO: Sum up the square of the distance to each of our existing neighbors */
  energy = 0.0f;
  for (unsigned int i = 0; i < MC_CUBE_NUM_FACES; ++i) {
    float distanceSquared;
    mcSurfaceNode *neighbor;
    neighbor = node->neighbors[i];
    if (neighbor == NULL)
      continue;
    distanceSquared = squared(neighbor->oldPos.x - node->pos.x)
      + squared(neighbor->oldPos.y - node->pos.y)
      + squared(neighbor->oldPos.z - node->pos.z);
    energy += distanceSquared;
  }
  return energy;
}

void mcElasticSurfaceNet_nodeNegihborsMidpoint(const mcSurfaceNode *node, mcVec3 *midpoint) {
  int numNeighbors;
  /* TODO: Average the position of all neighboring nodes */
  numNeighbors = 0;
  midpoint->x = 0.0f;
  midpoint->y = 0.0f;
  midpoint->z = 0.0f;
  for (unsigned int i = 0; i < MC_CUBE_NUM_FACES; ++i) {
    mcSurfaceNode *neighbor;
    neighbor = node->neighbors[i];
    if (neighbor == NULL)
      continue;
    numNeighbors += 1;
    midpoint->x += neighbor->pos.x;
    midpoint->y += neighbor->pos.y;
    midpoint->z += neighbor->pos.z;
  }
  if (numNeighbors == 0) {
    /* The midpoint is trivial for a node with no neighbors */
    *midpoint = node->pos;
    return;
  }
  midpoint->x /= (float)numNeighbors;
  midpoint->y /= (float)numNeighbors;
  midpoint->z /= (float)numNeighbors;
}

/**
 * This routine implements the Elastic Surface Net algorithm for extracting
 * isosurfaces as described by Gibson. This method does not converge very
 * quickly, and it does not necessarily follow the isosurface as closely as
 * other methods, but it should produce evenly distributed vertices.
 */
void mcElasticSurfaceNet_isosurfaceFromField(
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
  /* Build the surface net from samples of our scalar field */
  mcSurfaceNet_build(&surfaceNet,
      sf, args,
      res_x, res_y, res_z,
      min, max);
  /* TODO: Iteratively relax the position of surface nodes to reduce the total
   * energy between neighboring nodes */
  /* TODO: Make the number of iterations configurable */
  /* TODO: Automatically detect when the total energy is below a suitable threshold */
  static const int MAX_ITERATIONS = 3000;
  for (int i = 0; i < MAX_ITERATIONS; ++i) {
    /* TODO: Record the current position of the surface nodes as their old
     * positions */
    mcSurfaceNet_updateOldPos(&surfaceNet);
    /* TODO: We relax surface node positions by "nudging" surface nodes to
     * reduce the measured energy between nodes. Energy is measured as the sum
     * of the square distances between neighbors.
     */
    for (unsigned int j = 0; j < surfaceNet.numNodes; ++j) {
      mcSurfaceNode *node;
      mcVec3 initialPos, midpoint;
      float initial, final;
      node = mcSurfaceNet_getNode(&surfaceNet, j);
      /* TODO: Measure the initial energy of this node */
      initial = mcElasticSurfaceNet_nodeEnergy(node);
      /* TODO: Nudge the node towards the centerpoint of its neighbor nodes */
      mcElasticSurfaceNet_nodeNegihborsMidpoint(node, &midpoint);
      const float WEIGHT = 0.001f;
      node->pos.x = (1.0f - WEIGHT) * node->oldPos.x + WEIGHT * midpoint.x;
      node->pos.y = (1.0f - WEIGHT) * node->oldPos.y + WEIGHT * midpoint.y;
      node->pos.z = (1.0f - WEIGHT) * node->oldPos.z + WEIGHT * midpoint.z;
      /* Restrict the new position to within the voxel cube */
      node->pos.x = max(node->pos.x,
          min->x + (float)node->latticePos[0] * delta_x);
      node->pos.x = min(node->pos.x,
          min->x + (float)(node->latticePos[0] + 1) * delta_x);
      node->pos.y = max(node->pos.y,
          min->y + (float)node->latticePos[1] * delta_y);
      node->pos.y = min(node->pos.y,
          min->y + (float)(node->latticePos[1] + 1) * delta_y);
      node->pos.z = max(node->pos.z,
          min->z + (float)node->latticePos[2] * delta_z);
      node->pos.z = min(node->pos.z,
          min->z + (float)(node->latticePos[2] + 1) * delta_z);
      /* Measure the final energy of this node and compare */
      final = mcElasticSurfaceNet_nodeEnergy(node);
      if (final > initial) {
        node->pos = initialPos;
      }
    }
  }
  /* Now that the vertices are in their final positions, we can add them to the
   * mesh and generate vertex indices */
  mcFace_init(&triangle, 3);
  for (int i = 0; i < surfaceNet.numNodes; ++i) {
    mcSurfaceNode *node;
    mcVertex vertex;
    node = mcSurfaceNet_getNode(&surfaceNet, i);
    vertex.pos = node->pos;
    /* TODO: Calculate the surface normal */
    node->vertexIndex = mcMesh_addVertex(mesh, &vertex);
    /* XXX: Draw a triangle here so we can tell where the vertices are going */
    triangle.indices[0] = node->vertexIndex;
    vertex.pos.x += delta_x * 0.1;
    triangle.indices[1] = mcMesh_addVertex(mesh, &vertex);
    vertex.pos.y += delta_y * 0.1;
    triangle.indices[2] = mcMesh_addVertex(mesh, &vertex);
    mcMesh_addFace(mesh, &triangle);
  }
  /* With the vertex indices generated, we can now generate triangles */
  for (int i = 0; i < surfaceNet.numNodes; ++i) {
    mcSurfaceNode *node, *frontNeighbor, *leftNeighbor, *topNeighbor,
                  *bottomNeighbor, *rightNeighbor, *backNeighbor;
    node = mcSurfaceNet_getNode(&surfaceNet, i);
    /* We look for pairs of neighboring nodes in order to generate quads.  See
     * Gibson, "Constrained Elastic Surface Nets: Generating Smooth Models from
     * Binary Segmented Data."
     * Note that we must avoid generating redundant triangles, since each quad
     * has two possible triangulations. We make the decision arbitrarily. */
    frontNeighbor = node->neighbors[MC_SURFACE_NODE_FRONT];
    leftNeighbor = node->neighbors[MC_SURFACE_NODE_LEFT];
    topNeighbor = node->neighbors[MC_SURFACE_NODE_TOP];
    bottomNeighbor = node->neighbors[MC_SURFACE_NODE_BOTTOM];
    rightNeighbor = node->neighbors[MC_SURFACE_NODE_RIGHT];
    backNeighbor = node->neighbors[MC_SURFACE_NODE_BACK];
    /* TODO: Maybe add an option for changing the triangulation and/or chosing
     * an optimal triangulation. */
    /* TODO: Make sure the winding order of these triangles is correct */
    /* LEFT+FRONT and BACK+RIGHT or LEFT+BACK and FRONT+RIGHT */
    if (leftNeighbor && frontNeighbor
        && leftNeighbor->neighbors[MC_SURFACE_NODE_FRONT])
    {
      assert(leftNeighbor->neighbors[MC_SURFACE_NODE_FRONT]
          == frontNeighbor->neighbors[MC_SURFACE_NODE_LEFT]);
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = leftNeighbor->vertexIndex;
      triangle.indices[2] = frontNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    if (backNeighbor && rightNeighbor
        && backNeighbor->neighbors[MC_SURFACE_NODE_RIGHT])
    {
      assert(backNeighbor->neighbors[MC_SURFACE_NODE_RIGHT]
          == rightNeighbor->neighbors[MC_SURFACE_NODE_BACK]);
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = backNeighbor->vertexIndex;
      triangle.indices[2] = rightNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    /* FRONT+TOP and BOTTOM+BACK or FRONT+BOTTOM and TOP+BACK */
    if (frontNeighbor && topNeighbor
        && frontNeighbor->neighbors[MC_SURFACE_NODE_TOP])
    {
      assert(frontNeighbor->neighbors[MC_SURFACE_NODE_TOP]
          == topNeighbor->neighbors[MC_SURFACE_NODE_FRONT]);
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = frontNeighbor->vertexIndex;
      triangle.indices[2] = topNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    if (bottomNeighbor && backNeighbor
        && bottomNeighbor->neighbors[MC_SURFACE_NODE_BACK])
    {
      assert(bottomNeighbor->neighbors[MC_SURFACE_NODE_BACK]
          == backNeighbor->neighbors[MC_SURFACE_NODE_BOTTOM]);
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = bottomNeighbor->vertexIndex;
      triangle.indices[2] = backNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    /* LEFT+TOP and BOTTOM+RIGHT or LEFT+BOTTOM and TOP+RIGHT */
    if (leftNeighbor && topNeighbor
        && leftNeighbor->neighbors[MC_SURFACE_NODE_TOP])
    {
      assert(leftNeighbor->neighbors[MC_SURFACE_NODE_TOP]
          == topNeighbor->neighbors[MC_SURFACE_NODE_LEFT]);
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = leftNeighbor->vertexIndex;
      triangle.indices[2] = topNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
    if (bottomNeighbor && rightNeighbor
        && bottomNeighbor->neighbors[MC_SURFACE_NODE_RIGHT])
    {
      assert(bottomNeighbor->neighbors[MC_SURFACE_NODE_RIGHT]
          == rightNeighbor->neighbors[MC_SURFACE_NODE_BOTTOM]);
      triangle.indices[0] = node->vertexIndex;
      triangle.indices[1] = bottomNeighbor->vertexIndex;
      triangle.indices[2] = rightNeighbor->vertexIndex;
      mcMesh_addFace(mesh, &triangle);
    }
  }
  mcFace_destroy(&triangle);
}
