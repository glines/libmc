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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mc/algorithms/common/cube.h>
#include <mc/algorithms/nielsonDual/common.h>

#include "../common/canonical_cube_orientations.h"

#define get_byte(num, byte) (((num) & (0xff << (8 * byte))) >> (8 * byte))

void computeVertexList(int cube, mcNielsonDualVertexList *list) {
  int vertexIndex;

  /* Initialize the list with all values -1 */
  memset(list, -1, sizeof(mcNielsonDualVertexList));

  /* Determine this cube's canonical orientation and the rotation sequence that
   * brings it into that orientation */
  unsigned int canonical = mcCube_canonicalOrientation(cube);
  unsigned int sequence = mcCube_canonicalRotationSequence(cube);

  /* Generate the list of vertices for the given cube configuration */
  vertexIndex = 0;
#define vertex_3_edgeIntersections(a, b, c) \
  do { \
    int *ei = list->vertices[vertexIndex].edgeIntersections; \
    ei[0] = a; \
    ei[1] = b; \
    ei[2] = c; \
    for (int i = 3; i < MC_CUBE_NUM_EDGES; ++i) \
      ei[i] = -1; \
  } while(0)
#define vertex_4_edgeIntersections(a, b, c, d) \
  do { \
    int *ei = list->vertices[vertexIndex].edgeIntersections; \
    ei[0] = a; \
    ei[1] = b; \
    ei[2] = c; \
    ei[3] = d; \
    for (int i = 4; i < MC_CUBE_NUM_EDGES; ++i) \
      ei[i] = -1; \
  } while(0)
#define vertex_5_edgeIntersections(a, b, c, d, e) \
  do { \
    int *ei = list->vertices[vertexIndex].edgeIntersections; \
    ei[0] = a; \
    ei[1] = b; \
    ei[2] = c; \
    ei[3] = d; \
    ei[4] = e; \
    for (int i = 5; i < MC_CUBE_NUM_EDGES; ++i) \
      ei[i] = -1; \
  } while(0)
#define vertex_6_edgeIntersections(a, b, c, d, e, f) \
  do { \
    int *ei = list->vertices[vertexIndex].edgeIntersections; \
    ei[0] = a; \
    ei[1] = b; \
    ei[2] = c; \
    ei[3] = d; \
    ei[4] = e; \
    ei[5] = f; \
    for (int i = 6; i < MC_CUBE_NUM_EDGES; ++i) \
      ei[i] = -1; \
  } while(0)
#define vertex_3_connectivity(a, b, c) \
  do { \
    int *conn = list->vertices[vertexIndex].connectivity; \
    conn[0] = a; \
    conn[1] = b; \
    conn[2] = c; \
    for (int i = 3; i < MC_CUBE_NUM_FACES; ++i) \
      conn[i] = -1; \
  } while(0)
#define vertex_4_connectivity(a, b, c, d) \
  do { \
    int *conn = list->vertices[vertexIndex].connectivity; \
    conn[0] = a; \
    conn[1] = b; \
    conn[2] = c; \
    conn[3] = d; \
    for (int i = 4; i < MC_CUBE_NUM_FACES; ++i) \
      conn[i] = -1; \
  } while(0)
#define vertex_5_connectivity(a, b, c, d, e) \
  do { \
    int *conn = list->vertices[vertexIndex].connectivity; \
    conn[0] = a; \
    conn[1] = b; \
    conn[2] = c; \
    conn[3] = d; \
    conn[4] = e; \
    for (int i = 5; i < MC_CUBE_NUM_FACES; ++i) \
      conn[i] = -1; \
  } while(0)
#define vertex_6_connectivity(a, b, c, d, e, f) \
  do { \
    int *conn = list->vertices[vertexIndex].connectivity; \
    conn[0] = a; \
    conn[1] = b; \
    conn[2] = c; \
    conn[3] = d; \
    conn[4] = e; \
    conn[5] = f; \
    for (int i = 6; i < MC_CUBE_NUM_FACES; ++i) \
      conn[i] = -1; \
  } while(0)
  switch (canonical) {
    case MC_CUBE_CANONICAL_ORIENTATION_0:
      /* This is the case where all samples are above the isosurface. Not very
       * interesting. */
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_1:
      /* This is a simple case with only one sample below the isosurface. This
       * generates a single vertex which is connected to the three faces at
       * that corner. */
      vertex_3_edgeIntersections(0, 8, 3);
      vertex_3_connectivity(0, 3, 4);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_2:
      /* This is the case where two samples on the same voxel edge are below
       * the isosurface. This generates a single vertex which is connected to
       * the four faces near that edge. */
      vertex_4_edgeIntersections(1, 9, 8, 3);
      vertex_4_connectivity(0, 1, 3, 4);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_3:
      /* This case has two sample points on the same face that are below the
       * isosurface. These samples are diagonal from each other, so this is an
       * ambiguous face. */
      vertex_3_edgeIntersections(0, 8, 3);
      vertex_3_connectivity(0, 3, 4);
      ++vertexIndex;
      vertex_3_edgeIntersections(1, 2, 11);
      vertex_3_connectivity(0, 1, 2);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_4:
      /* This case has three sample points below the surface that form an "L"
       * shape on one face. This generates a single vertex that connects with
       * that face as well as the four adjacent faces. */
      vertex_5_edgeIntersections(2, 11, 9, 8, 3);
      vertex_5_connectivity(0, 1, 2, 3, 4);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_5:
      /* This case has four samples on the front face below the isosurface and
       * four samples on the back face above the isosurface. This generates a
       * single vertex that connects with the four faces adjacent to the front
       * face. */
    vertex_4_edgeIntersections(8, 10, 11, 9);
    vertex_4_connectivity(1, 2, 3, 4);
    ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_6:
      /* This case has two samples below the isosurface on opposite corners of
       * the cube. This generates two separate vertices near these samples
       * which collectively connect to all cube faces. */
      vertex_3_edgeIntersections(1, 2, 11);
      vertex_3_connectivity(0, 1, 2);
      ++vertexIndex;
      vertex_3_edgeIntersections(4, 7, 8);
      vertex_3_connectivity(3, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_7:
      /* This case has two samples below the isosurface that share an edge and
       * a third sample in a corner diagonal to this edge on the front face,
       * creating an ambiguous face. This generates two vertices, one of which
       * is connected to the faces near that edge, and the other connected to
       * the faces on that corner. */
      vertex_4_edgeIntersections(0, 4, 7, 3);
      vertex_4_connectivity(0, 3, 4, 5);
      ++vertexIndex;
      vertex_3_edgeIntersections(1, 2, 11);
      vertex_3_connectivity(0, 1, 2);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_8:
      /* This case has four samples below the isosurface that form a serpentine
       * "Z" shape when following the voxel cube edges viewed from the outside
       * of the isosurface. This generates a single vertex that connects
       * through all voxel cube faces. */
      vertex_6_edgeIntersections(2, 11, 9, 4, 7, 3);
      vertex_6_connectivity(0, 1, 2, 3, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_9:
      /* This case has the three samples surrounding vertex zero below the
       * isosurface. This generates three separate vertices in the corners with
       * those samples that are each connected to the three faces in their
       * respective corner. */
      vertex_3_edgeIntersections(0, 1, 9);
      vertex_3_connectivity(0, 1, 3);
      ++vertexIndex;
      vertex_3_edgeIntersections(2, 3, 10);
      vertex_3_connectivity(0, 2, 4);
      ++vertexIndex;
      vertex_3_edgeIntersections(4, 7, 8);
      vertex_3_connectivity(3, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_10:
      /* This case has the four samples nearest vertex zero below the
       * isosurface. This makes a hexagon shape where it intsersects the voxel
       * cube. This generates a single vertex that connects through all voxel
       * faces. */
      vertex_6_edgeIntersections(1, 9, 4, 7, 10, 2);
      vertex_6_connectivity(0, 1, 2, 3, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_11:
      /* This case has four samples below the isosurface that form a serpentine
       * "S" shape when following the voxel cube edges viewed from the outside
       * of the isosurface. This generates a single vertex that connects
       * through all voxel cube faces. */
      vertex_6_edgeIntersections(0, 4, 7, 10, 11, 1);
      vertex_6_connectivity(0, 1, 2, 3, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_12:
      /* This case has three samples below the isosurface that form an L shape
       * on one face, and a fourth sample in the corner apart from the other
       * three. This generates two vertices. */
      vertex_5_edgeIntersections(0, 3, 10, 11, 9);
      vertex_5_connectivity(0, 1, 2, 3, 4);
      ++vertexIndex;
      vertex_3_edgeIntersections(4, 7, 8);
      vertex_3_connectivity(3, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_13:
      /* This case has all four samples on the front face below the isosurface,
       * as well as a fifth sample on the back face that is below the
       * isosurface. The vertices above the isosurface form an "L" shape on one
       * face. One vertex is generated which connects to the faces near the
       * face with the vertices above the isosurface. This is analogous to
       * MC_CUBE_CANONICAL_ORIENTATION_4. */
      vertex_5_edgeIntersections(4, 7, 10, 11, 9);
      vertex_5_connectivity(1, 2, 3, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_14:
      /* This case has two samples on the same voxel edge below the isosurface,
       * and an additional two samples below the isosurface on the edge
       * diagonal to the first edge. This generates two vertices. These
       * vertices connect with the fourfaces nearest their respective edges. */
      vertex_4_edgeIntersections(1, 3, 10, 11);
      vertex_4_connectivity(0, 1, 2, 4);
      ++vertexIndex;
      vertex_4_edgeIntersections(5, 7, 8, 9);
      vertex_4_connectivity(1, 3, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_15:
      /* This case has two samples along the same voxel edge above the
       * isosurface, and a third sample in a corner diagonal to the first two
       * also above the isosurface. This generates two vertices, the first is
       * connected to the faces near that edge, and the second is connected to
       * the faces near that corner. */
      vertex_4_edgeIntersections(5, 7, 10, 11);
      vertex_4_connectivity(1, 2, 4, 5);
      ++vertexIndex;
      vertex_3_edgeIntersections(0, 9, 1);
      vertex_3_connectivity(0, 1, 3);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_16:
      /* This case has two samples on the same edge which are above the
       * isosurface. This generates a single vertex which is connected to the
       * faces around that edge. */
      vertex_4_edgeIntersections(5, 7, 10, 11);
      vertex_4_connectivity(1, 2, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_17:
      /* This case has four samples on separate voxel cube corners below the
       * isosurface. This generates four separate vertices, each connecting to
       * the three faces adjacent to their respective sample point. */
      vertex_3_edgeIntersections(0, 1, 9);
      vertex_3_connectivity(0, 1, 3);
      ++vertexIndex;
      vertex_3_edgeIntersections(2, 3, 10);
      vertex_3_connectivity(0, 2, 4);
      ++vertexIndex;
      vertex_3_edgeIntersections(4, 7, 8);
      vertex_3_connectivity(3, 4, 5);
      ++vertexIndex;
      vertex_3_edgeIntersections(5, 11, 6);
      vertex_3_connectivity(1, 2, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_18:
      /* This case has three samples above the isosurface in separate corners
       * of the voxel cube. This generates three vertices which connect to the
       * faces in their respective corners. */
      vertex_3_edgeIntersections(1, 11, 2);
      vertex_3_connectivity(0, 1, 2);
      ++vertexIndex;
      vertex_3_edgeIntersections(4, 5, 9);
      vertex_3_connectivity(1, 3, 5);
      ++vertexIndex;
      vertex_3_edgeIntersections(6, 7, 10);
      vertex_3_connectivity(2, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_19:
      /* This case has two sample points on the same face diagonal from each
       * other above the isosurface. This generates two vertices which connect
       * through the faces in their respective corners. */
      vertex_3_edgeIntersections(4, 5, 9);
      vertex_3_connectivity(1, 3, 5);
      ++vertexIndex;
      vertex_3_edgeIntersections(6, 7, 10);
      vertex_3_connectivity(2, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_20:
      /* This case has two sample points above the isosurface which are not on
       * the same face. This generates two vertices which connect through the
       * voxel faces in their respective corners. */
      vertex_3_edgeIntersections(0, 9, 1);
      vertex_3_connectivity(0, 1, 3);
      ++vertexIndex;
      vertex_3_edgeIntersections(6, 7, 10);
      vertex_3_connectivity(2, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_21:
      /* This case has a single sample point above the isosurface. This
       * generates a single vertex which connects through the faces adjacent to
       * that sample point. */
      vertex_3_edgeIntersections(6, 7, 10);
      vertex_3_connectivity(2, 4, 5);
      ++vertexIndex;
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_22:
      /* This is the case where all samples are below the isosurface. Not very
       * interesting. */
      break;
  }

  /* Rotate the edge intersections and face connectivities into their true
   * positions */
  for (int i = 0; i < MC_NIELSON_DUAL_MAX_VERTICES; ++i) {
    mcNielsonDualVertex *vertex;
    vertex = &list->vertices[i];
    if (vertex->edgeIntersections[0] == -1)
      break;  /* No more vertices to consider */
    /* Rotate the edge intersections back */
    for (int j = 0; j < MC_CUBE_NUM_EDGES; ++j) {
      if (vertex->edgeIntersections[j] == -1)
        break;  /* No more edge intersections to consider */
      /* Rotate the edge intersection about the y-axis */
      for (int k = 0; k < get_byte(sequence, 2); ++k) {
        vertex->edgeIntersections[j] =
          mcCube_rotateEdgeReverseY(vertex->edgeIntersections[j]);
      }
      /* Rotate the edge intersection about the x-axis */
      for (int k = 0; k < get_byte(sequence, 1); ++k) {
        vertex->edgeIntersections[j] =
          mcCube_rotateEdgeReverseX(vertex->edgeIntersections[j]);
      }
      /* Rotate the edge intersection about the z-axis */
      for (int k = 0; k < get_byte(sequence, 0); ++k) {
        vertex->edgeIntersections[j] =
          mcCube_rotateEdgeReverseZ(vertex->edgeIntersections[j]);
      }
    }
    /* TODO: Rotate the face connections back */
    /* NOTE: We might not need the face connectivity. I have not found a use
     * for it yet. */
  }
}

void computeVertexTable(mcNielsonDualVertexList *table) {
  /* Iterate over all voxel cube configurations */
  for (int cube = 0; cube <= 0xff; ++cube) {
    /* Compute the vertex list for this cube configuration */
    computeVertexList(cube, &table[cube]);
  }
}

void computeMidpointVertexList(
    const mcNielsonDualVertexList *vertexList,
    mcNielsonDualCookedVertexList *midpointVertexList)
{
  /* Initialize the number of vertices in the midpoint vertex list */
  midpointVertexList->numVertices = 0;
  /* Iterate over the vertices in the vertex list */
  for (int i = 0; i < MC_NIELSON_DUAL_MAX_VERTICES; ++i) {
    const mcNielsonDualVertex *vertex;
    if (vertexList->vertices[i].edgeIntersections[0] == -1)
      break;  /* No more vertices to consider */
    midpointVertexList->numVertices += 1;
    vertex = &vertexList->vertices[i];
    /* Compute the midpoint of the edge intersections, assuming that all edge
     * intersections occur in the middle of each respective edge. */
    int numEdgeIntersections = 0;
    mcVec3 midpoint;
    midpoint.x = midpoint.y = midpoint.z = 0.0f;
    for (int j = 0; j < MC_CUBE_NUM_EDGES; ++j) {
      unsigned int edge, sampleIndices[2];
      mcVec3 vertexPoints[2], edgeMidpoint;
      if (vertex->edgeIntersections[j] == -1)
        break;  /* No more edge intersections to consider */
      numEdgeIntersections += 1;
      edge = vertex->edgeIntersections[j];
      /* Compute the position of the midpoint of this edge on the unit cube
       * defined by the points (0.0, 0.0, 0.0) and (1.0, 1.0, 1.0) */
      mcCube_edgeSampleIndices(edge, sampleIndices);
      for (int k = 0; k < 2; ++k) {
        int pos[3];
        mcCube_sampleRelativePosition(sampleIndices[k], (unsigned int*)pos);
        vertexPoints[k].x = pos[0] ? 1.0f : 0.0f;
        vertexPoints[k].y = pos[1] ? 1.0f : 0.0f;
        vertexPoints[k].z = pos[2] ? 1.0f : 0.0f;
      }
      edgeMidpoint = mcVec3_lerp(&vertexPoints[0], &vertexPoints[1], 0.5f);
      mcVec3_add(&midpoint, &edgeMidpoint, &midpoint);
    }
    mcVec3_scalarProduct(
        (1.0f / (float)numEdgeIntersections), &midpoint, &midpoint);
    /* Store the computed midpoint as the cooked vertex position */
    midpointVertexList->vertices[i].pos = midpoint;
    /* We estimate the surface normal at this vertex by calculating the normal
     * of the surface patch. Note that because the surface patch vertices are
     * not necessarily co-planar, we can get a better estimate by averaging the
     * normals for more than one triangle.
     */
    mcVec3 normal;
    int numTriangles = 0;
    normal.x = normal.y = normal.z = 0.0f;
    assert(vertex->edgeIntersections[0] != -1);
    assert(vertex->edgeIntersections[1] != -1);
    assert(vertex->edgeIntersections[2] != -1);
    /* Iterate through triangles in the triangle fan that makes this patch and
     * compute all of their normals */
    for (int j = 0; j + 2 < MC_CUBE_NUM_EDGES; ++j) {
      if (vertex->edgeIntersections[j + 2] == -1)
        break;  /* No more triangles to consider */
      numTriangles += 1;
      mcVec3 points[3];
      /* Compute the three points of this triangle */
      for (int k = 0; k < 3; ++k) {
        int edge;
        unsigned int sampleIndices[2];
        mcVec3 vertices[2];
        if (k == 0)
          edge = vertex->edgeIntersections[0];
        else
          edge = vertex->edgeIntersections[j + k];
        /* The edge intersection point is halfway between the cube samples */
        mcCube_edgeSampleIndices(edge, sampleIndices);
        for (int l = 0; l < 2; ++l) {
          unsigned int pos[3];
          mcCube_sampleRelativePosition(sampleIndices[l], pos);
          vertices[l].x = pos[0] ? 1.0f : 0.0f;
          vertices[l].y = pos[1] ? 1.0f : 0.0f;
          vertices[l].z = pos[2] ? 1.0f : 0.0f;
        }
        points[k] = mcVec3_lerp(&vertices[0], &vertices[1], 0.5f);
      }
      mcVec3 tangent[2], triangleNormal;
      /* Compute the tangent vectors and normal */
      mcVec3_subtract(&points[1], &points[0], &tangent[0]);
      mcVec3_subtract(&points[2], &points[0], &tangent[1]);
      mcVec3_cross(&tangent[1], &tangent[0], &triangleNormal);
      mcVec3_normalize(&triangleNormal, &triangleNormal);
      mcVec3_add(&normal, &triangleNormal, &normal);
    }
    /* Average the triangle normals for our vertex normal */
    mcVec3_scalarProduct(1.0f / (float)numTriangles, &normal, &normal);
    midpointVertexList->vertices[i].norm = normal;
  }
}

void computeMidpointVertexTable(
    const mcNielsonDualVertexList *vertexTable,
    mcNielsonDualCookedVertexList *midpointVertexTable)
{
  /* Iterate over all voxel cube configurations */
  for (int cube = 0; cube <= 0xff; ++cube) {
    /* Compute the midpoint vertex list for this cube configuration */
    computeMidpointVertexList(&vertexTable[cube], &midpointVertexTable[cube]);
  }
}

void computeVertexIndexLookupTable(
    const mcNielsonDualVertexList *vertexTable,
    int *lookupTable)
{
  /* Iterate over all cube edges */
  for (int edge = 0; edge < MC_CUBE_NUM_EDGES; ++edge) {
    /* Iterate over all voxel cube configurations */
    for (int cube = 0; cube <= 0xff; ++cube) {
      const mcNielsonDualVertexList *list;
      int vertexIndex;
      /* Look up the vertex information for this cube configuration */
      list = &vertexTable[cube];
      /* Look for the vertex associated with this edge */
      vertexIndex = -1;
      for (int i = 0; i < MC_NIELSON_DUAL_MAX_VERTICES; ++i) {
        const mcNielsonDualVertex *vertex;
        vertex = &list->vertices[i];
        if (vertex->edgeIntersections[0] == -1)
          break;  /* No more vertices to consider */
        /* Iterate over the edge intersections for this vertex */
        for (int j = 0; j < MC_CUBE_NUM_EDGES; ++j) {
          if (vertex->edgeIntersections[j] == -1)
            break;  /* No more edge intersections to consider */
          if (vertex->edgeIntersections[j] == edge) {
            if (vertexIndex == -1) {
              /* We found the vertex that has a face intersecting this edge */
              vertexIndex = i;
            } else {
              /* Two vertices cannot have faces intersecting the same edge */
              assert(0);
            }
          }
        }
      }
      /* Store the vertex index in the lookup table */
      lookupTable[(edge << 8) + cube] = vertexIndex;
    }
  }
}

void computeWindingTable(int *table)
{
  /* NOTE: This table is larger than it needs to be. We only use two bits from
   * the cube configuration index. */
  /* Iterate over all cube edges */
  for (int edge = 0; edge < MC_CUBE_NUM_EDGES; ++edge) {
    /* Iterate over all voxel cube configurations */
    for (int cube = 0; cube <= 0xff; ++cube) {
      unsigned int sampleIndices[2], values[2];
      /* TODO: Determine the sign of this edge */
      /* NOTE: The edge samples are always given from least to greatest sample
       * index. */
      mcCube_edgeSampleIndices(edge, sampleIndices);
      for (int i = 0; i < 2; ++i) {
        values[i] = mcCube_sampleValue(sampleIndices[i], cube);
      }
      if (values[0] == values[1]) {
        /* No isosurface intersection at this edge */
        table[(edge << 8) + cube] = -1;
        continue;
      }
      /* FIXME: I'm very uncertain about the convention used for encoding
       * isosurface sign of the cube configuration here. I thought this
       * inequality should be the other way. Maybe something is backwards
       * around here. */
      int sign = values[1] > values[0] ? 1 : 0;
      unsigned int faces[2];
      mcCube_edgeFaces(edge, faces);
      if (sign) {
        table[(edge << 8) + cube] = faces[0];
      } else {
        table[(edge << 8) + cube] = faces[1];
      }
    }
  }
}

void printVertexTable(const mcNielsonDualVertexList *table) {
  fprintf(stdout, "const mcNielsonDualVertexList mcNielsonDual_vertexTable[] = {\n");
  /* Iterate over all cubes in the table */
  for (int cube = 0; cube <= 0xff; ++cube) {
    fprintf(stdout,
        "  {\n"
        "    .vertices = {\n");
    /* Iterate over the list of vertices */
    for (int vertex = 0; vertex < MC_NIELSON_DUAL_MAX_VERTICES; ++vertex) {
      fprintf(stdout,
          "      {\n");
      /* Print the edge intersections */
      fprintf(stdout,
          "        .edgeIntersections = { ");
      for (int i = 0; i < MC_CUBE_NUM_EDGES; ++i) {
        fprintf(stdout, "%2d, ",
            table[cube].vertices[vertex].edgeIntersections[i]);
      }
      fprintf(stdout, "},\n");
      /* Print the face connectivity */
      fprintf(stdout,
          "        .connectivity = { ");
      for (int i = 0; i < MC_CUBE_NUM_FACES; ++i) {
        fprintf(stdout, "%2d, ",
            table[cube].vertices[vertex].connectivity[i]);
      }
      fprintf(stdout, "},\n");
      fprintf(stdout,
          "      },\n");
    }
    fprintf(stdout,
        "    },\n"
        "  },\n");
  }
  fprintf(stdout,
      "};\n");
}

void printMidpointVertexTable(mcNielsonDualCookedVertexList *table) {
  fprintf(stdout,
      "const mcNielsonDualCookedVertexList "
      "mcNielsonDual_midpointVertexTable[] = {\n");
  for (int cube = 0; cube <= 0xff; ++cube) {
    fprintf(stdout,
        "  {\n"
        "    .vertices = {\n");
    /* Print the vertices */
    for (int i = 0; i < MC_NIELSON_DUAL_MAX_VERTICES; ++i) {
      fprintf(stdout,
          "      {\n");
      /* Print the vertex position */
      fprintf(stdout,
          "        .pos = {\n"
          "          .x = %f,\n"
          "          .y = %f,\n"
          "          .z = %f,\n"
          "        },\n",
          table[cube].vertices[i].pos.x,
          table[cube].vertices[i].pos.y,
          table[cube].vertices[i].pos.z
          );
      /* Print the surface normal */
      fprintf(stdout,
          "        .norm = {\n"
          "          .x = %f,\n"
          "          .y = %f,\n"
          "          .z = %f,\n"
          "        },\n",
          table[cube].vertices[i].norm.x,
          table[cube].vertices[i].norm.y,
          table[cube].vertices[i].norm.z
          );
      fprintf(stdout,
          "      },\n");
    }
    fprintf(stdout,
        "    },\n");
    /* Print the number of vertices */
    fprintf(stdout, "    .numVertices = %d,\n",
        table[cube].numVertices);
    fprintf(stdout,
        "  },\n");
  }
  fprintf(stdout, "};\n");
}

void printVertexIndexLookupTable(int *table) {
  fprintf(stdout, "const int mcNielsonDual_vertexIndexLookupTable[] = {\n");
  for (int i = 0; i < MC_CUBE_NUM_EDGES * 0x100; ++i) {
    fprintf(stdout, "  %2d,  /* Edge: %d, Cube: 0x%02x */\n",
        table[i], i >> 8, i & 0xff);
  }
  fprintf(stdout, "};\n");
}

void printWindingTable(int *table) {
  fprintf(stdout, "const int mcNielsonDual_windingTable[] = {\n");
  for (int i = 0; i < MC_CUBE_NUM_EDGES * 0x100; ++i) {
    fprintf(stdout, "  %2d,  /* Edge: %d, Cube: 0x%02x */\n",
        table[i], i >> 8, i & 0xff);
  }
  fprintf(stdout, "};\n");
}

int main(int argc, char **argv) {
  mcNielsonDualVertexList *vertexTable;
  mcNielsonDualCookedVertexList *midpointVertexTable;
  int *vertexIndexLookupTable, *windingTable;

  /* Allocate for memory our tables */
  vertexTable = (mcNielsonDualVertexList*)malloc(
      sizeof(mcNielsonDualVertexList) * 256);
  midpointVertexTable = (mcNielsonDualCookedVertexList*)malloc(
      sizeof(mcNielsonDualCookedVertexList) * 256);
  vertexIndexLookupTable = (int*)malloc(
      sizeof(int) * MC_CUBE_NUM_EDGES * 256);
  windingTable = (int*)malloc(
      sizeof(int) * MC_CUBE_NUM_EDGES * 256);

  /* Compute the tables */
  computeVertexTable(vertexTable);
  computeMidpointVertexTable(vertexTable, midpointVertexTable);
  computeVertexIndexLookupTable(vertexTable, vertexIndexLookupTable);
  computeWindingTable(windingTable);

  /* Print the tables */
  printVertexTable(vertexTable);
  fprintf(stdout, "\n");
  printMidpointVertexTable(midpointVertexTable);
  fprintf(stdout, "\n");
  printVertexIndexLookupTable(vertexIndexLookupTable);
  fprintf(stdout, "\n");
  printWindingTable(windingTable);

  /* Free table memory */
  free(windingTable);
  free(vertexIndexLookupTable);
  free(midpointVertexTable);
  free(vertexTable);
}
