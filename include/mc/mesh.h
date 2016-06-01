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

void mcFace_init(
    mcFace *self,
    unsigned int numIndices);

void mcFace_copy(
    mcFace *self,
    const mcFace *other);

void mcFace_move(
    mcFace *self,
    mcFace *other);

void mcFace_destroy(
    mcFace *self);

typedef struct mcMesh {
  mcVertex *vertices;
  mcFace *faces;
  unsigned int numVertices, numFaces;
  unsigned int sizeVertices, sizeFaces;
} mcMesh;

void mcMesh_init(
    mcMesh *self);

void mcMesh_destroy(
    mcMesh *self);

void mcMesh_growVertices(
    mcMesh *self);

void mcMesh_growFaces(
    mcMesh *self);

unsigned int mcMesh_addVertex(
    mcMesh *self,
    const mcVertex *vertex);

void mcMesh_addFace(
    mcMesh *self,
    const mcFace *face);

#endif
