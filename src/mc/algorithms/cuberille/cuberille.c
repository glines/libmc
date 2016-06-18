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

#include <stdio.h> /* XXX */

#include <mc/algorithms/common/surfaceNet.h>
#include <mc/algorithms/cuberille/cuberille.h>
#include <mc/mesh.h>

void mcCuberille_isosurfaceFromField(
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int res_x, unsigned int res_y, unsigned int res_z,
    const mcVec3 *min, const mcVec3 *max,
    mcCuberilleParams *params,
    mcMesh *mesh)
{
  /* FIXME: Read the params */
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
  fprintf(stderr, "numNodes: %d\n", surfaceNet.numNodes);
  /* Now that we have the surface net, we generate a vertex for each surface
   * node */
  mcFace_init(&triangle, 3);
  for (int i = 0; i < surfaceNet.numNodes; ++i) {
    mcSurfaceNode *node;
    mcVertex vertex;
    node = mcSurfaceNet_getNode(&surfaceNet, i);
    vertex.pos = node->pos;
    /* TODO: Calculate the surface normal */
    /* NOTE: Since cubes have sharp corners, we need multiple vertices in
     * strategic places in order to accurately represent the sharp angles.
     * Corners require three vertices, edges require two vertices, and flat
     * surfaces require only one vertex.
     *
     * TODO: In order to classify vertices, we can use a lookup table. For
     * instance, given the two neighbors nodes that make a triangle, we can
     * determine the direction the normal points. Given the neighbor
     * connectivity in all 6 directions, we can determine the type of node
     * (corner, edge, or flat) and also all of the normal directions we need to
     * compute.
     *
     * TODO: There are many reasonable strategies for generating a cuberille
     * mesh. We can generate triangles or quads.
     */
    /* Add the vertex for this node to the mesh */
    node->vertexIndex = mcMesh_addVertex(mesh, &vertex);
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
  /* Free our resources */
  mcFace_destroy(&triangle);
  mcSurfaceNet_destroy(&surfaceNet);
}
