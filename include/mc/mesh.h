#ifndef MC_MESH_H_
#define MC_MESH_H_

typedef struct mcVertex {
  float pos[3];
  float norm[3];
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

#endif
