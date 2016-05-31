#ifndef MC_MESH_H_
#define MC_MESH_H_

#include "vector.h"

typedef struct mcVertex {
  mcVec3 pos;
  mcVec3 norm;
} mcVertex;

typedef struct mcFace {
  unsigned int *indices;
  unsigned int numIndices;
} mcFace;

typedef struct mcMesh {
  mcVertex *vertices;
  mcFace *faces;
  unsigned int numVertices, numFaces;
} mcMesh;

void mcMesh_init(
    mcMesh *self);

void mcMesh_destroy(
    mcMesh *self);

void mcMesh_grow(
    mcMesh *self);

void mcMesh_addVertex(
    mcMesh *self,
    const mcVertex *vertex);

void mcMesh_addFace(
    mcMesh *self,
    const mcFace *face);

#endif
