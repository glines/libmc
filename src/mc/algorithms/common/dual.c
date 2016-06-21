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

#include <mc/algorithms/common/dual.h>

/**
 * \internal
 * A struct to store a vertex index and the angle the vertex makes relative to
 * the first vertex in the patch/polygon.
 * \endinternal
 */
typedef struct VertexAngle {
  /** The vertex index of the vertex being considered. */
  int vertexIndex;
  /** The angle that this vertex makes relative to the first vertex in the
   * patch/polygon. */
  float angle;
} VertexAngle;

/**
 * \internal
 * Comparison routine for sorting vertices by angle for correct winding order.
 */
int compVertexAngle(const VertexAngle *u, const VertexAngle *v) {
  float theta_u, theta_v;
  theta_u = u->angle;
  if (theta_u < 0.0f)
    theta_u += 2.0f * (float)M_PI;
  theta_v = v->angle;
  if (theta_v < 0.0f)
    theta_v += 2.0f * (float)M_PI;
  if (theta_u < theta_v)
    return -1;
  if (theta_u > theta_v)
    return 1;
  return 0;
}

/**
 * Constructs the dual mesh of a given mesh.
 */
void mcDual_makeDual(
    const mcMesh *mesh,
    int maxFacesPerVertex,
    mcMesh *dual)
{
  maxFacesPerVertex += 42;  /* XXX */
  /* TODO: Allocate space to record faces related by their shared vertices */
  /* TODO: We could use a struct here with pointers to a single pool of vertex faces */
  int *adjacentFaces = (int*)malloc(sizeof(int) * mesh->numVertices * (maxFacesPerVertex + 1));
  memset(adjacentFaces, 0, sizeof(int) * mesh->numVertices * (maxFacesPerVertex + 1));
  /* TODO: Iterate over the faces to determine their connectivity */
  for (int i = 0; i < mesh->numFaces; ++i) {
    mcFace *face;
    face = &mesh->faces[i];
    /* TODO: Iterate over the face vertex indices to gather vertex faces */
    for (int j = 0; j < face->numIndices; ++j) {
      int *numAdjacentFaces;
      int vertexIndex = face->indices[j];
      numAdjacentFaces = &adjacentFaces[vertexIndex * (maxFacesPerVertex + 1)];
      assert(*numAdjacentFaces < maxFacesPerVertex);
      adjacentFaces[vertexIndex * (maxFacesPerVertex + 1) + 1 + *numAdjacentFaces] = i;
      *numAdjacentFaces += 1;
    }
  }
  /* We cache the resulting vertex index for each face midpoint so that we
   * don't end up with polygon soup */
  int *midpoints = (int*)malloc(sizeof(int) * mesh->numFaces);
  memset(midpoints, -1, sizeof(int) * mesh->numFaces);
  /* Iterate over the mesh faces; each face will produce a vertex in the dual
   * mesh */
  for (int i = 0; i < mesh->numFaces; ++i) {
    mcFace *face = &mesh->faces[i];
    mcVertex midpoint;
    /* Compute the midpoint as the average of this face's vertices */
    midpoint.pos.x = midpoint.pos.y = midpoint.pos.z = 0.0f;
    midpoint.norm.x = midpoint.norm.y = midpoint.norm.z = 0.0f;
    for (int j = 0; j < face->numIndices; ++j) {
      mcVertex *vertex = &mesh->vertices[face->indices[j]];
      midpoint.pos.x += vertex->pos.x;
      midpoint.pos.y += vertex->pos.y;
      midpoint.pos.z += vertex->pos.z;
      midpoint.norm.x += vertex->norm.x;
      midpoint.norm.y += vertex->norm.y;
      midpoint.norm.z += vertex->norm.z;
    }
    midpoint.pos.x /= (float)face->numIndices;
    midpoint.pos.y /= (float)face->numIndices;
    midpoint.pos.z /= (float)face->numIndices;
    midpoint.norm.x /= (float)face->numIndices;
    midpoint.norm.y /= (float)face->numIndices;
    midpoint.norm.z /= (float)face->numIndices;
    /* Add the midpoint computed to the dual mesh */
    midpoints[i] = mcMesh_addVertex(dual, &midpoint);
  }
  /* TODO: Iterate over the mesh vertices; each mesh vertex will produce a face
   * in the dual mesh */
  for (int i = 0; i < mesh->numVertices; ++i) {
    int numAdjacentFaces;
    int adjacentFaceIndex, midpointIndex;
    mcFace face;
    mcVec3 midpointVector, referenceHand;
    mcVec3 *meshPos, *meshNorm, *midpoint;
    VertexAngle *angles;
    /* TODO: Iterate over the faces adjacent to this vertex */
    numAdjacentFaces = adjacentFaces[i * (maxFacesPerVertex + 1)];
    if (numAdjacentFaces < 3) {
      /* Don't generate faces for points or lines */
      continue;
    }
    mcFace_init(&face, numAdjacentFaces);
    /* TODO: We determine the winding order of the vertices on the dual mesh
     * face by sorting the angles that the midpoints make with regards to the
     * surface normal on the  */
    meshPos = &mesh->vertices[i].pos;
    meshNorm = &mesh->vertices[i].norm;
    adjacentFaceIndex = adjacentFaces[i * (maxFacesPerVertex + 1) + 1];
    midpointIndex = midpoints[adjacentFaceIndex];
    midpoint = &dual->vertices[midpointIndex].pos;
    mcVec3_subtract(midpoint, meshPos, &midpointVector);
    mcVec3_cross(meshNorm, &midpointVector, &referenceHand);
    mcVec3_normalize(&referenceHand, &referenceHand);
    /* FIXME: Get rid of this malloc */
    angles = (VertexAngle*)malloc(sizeof(VertexAngle) * numAdjacentFaces);
    angles[0].vertexIndex = midpointIndex;
    angles[0].angle = 0.0f;
    for (int j = 1; j < numAdjacentFaces; ++j) {
      int faceIndex;
      mcVec3 currentHand, cross;
      float sinTheta, cosTheta, angle;
      /* TODO: Determine the angle that this vector makes relative reference
       * hand and the mesh normal vector  */
      faceIndex = adjacentFaces[i * (maxFacesPerVertex + 1) + 1 + j];
      midpointIndex = midpoints[faceIndex];
      midpoint = &dual->vertices[midpoints[faceIndex]].pos;
      mcVec3_subtract(midpoint, meshPos, &midpointVector);
      mcVec3_cross(meshNorm, &midpointVector, &currentHand);
      mcVec3_normalize(&currentHand, &currentHand);
      mcVec3_cross(&referenceHand, &currentHand, &cross);
      sinTheta = mcVec3_length(&cross);
      cosTheta = mcVec3_dot(&referenceHand, &currentHand);
      angle = atan2(sinTheta, cosTheta);
      angles[j].vertexIndex = midpointIndex;
      angles[j].angle = angle;
    }
    /* TODO: Sort the vertices on the dual mesh face by angle so that they are
     * in the correct winding order */
    qsort(angles, numAdjacentFaces, sizeof(VertexAngle),
        (int (*)(const void *, const void *))compVertexAngle);
    /* TODO: Add the vertices to our new dual mesh face in correct winding
     * order */
    for (int j = 0; j < numAdjacentFaces; ++j) {
      face.indices[j] = angles[j].vertexIndex;
    }
    /* Add the face to the dual mesh */
    mcMesh_addFace(dual, &face);
    /* FIXME: We need a pool so that freeing this face memory is cheap */
    mcFace_destroy(&face);
  }
}
