#ifndef MC_ISOSURFACE_BUILDER_H_
#define MC_ISOSURFACE_BUILDER_H_

#include <stdint.h>

#include "mesh.h"

typedef enum mcAlgorithmFlag {
  MC_SIMPLE_MARCHING_CUBES = 1,
  MC_ELASTIC_SURFACE_NETS,
  MC_SNAP_MARCHING_CUBES,
} mcAlgorithmFlag;

typedef float (*mcScalarField)(float x, float y, float z);

typedef struct mcScalarLattice {
  float *lattice;
  unsigned int size[3];
  float delta[3];
} mcScalarLattice;

typedef struct mcScalarPoint {
  float pos[3];
  float value;
} mcScalarPoint;

typedef struct mcScalarCloud {
  mcScalarPoint *cloud;
  unsigned int size;
} mcScalarCloud;

typedef struct mcIsosurfaceBuilderInternal mcIsosurfaceBuilderInternal;

typedef struct mcIsosurfaceBuilder {
  mcIsosurfaceBuilderInternal *internal;
} mcIsosurfaceBuilder;

void mcIsosurfaceBuilder_init(
    mcIsosurfaceBuilder *self);

void mcIsosurfaceBuilder_destroy(
    mcIsosurfaceBuilder *self);

const mcMesh *mcIsosurfaceBuilder_isosurfaceFromField(
    mcIsosurfaceBuilder *self,
    mcScalarField sf,
    mcAlgorithmFlag algorithm);

const mcMesh *mcIsosurfaceBuilder_isosurfaceFromLattice(
    mcIsosurfaceBuilder *self,
    mcScalarLattice sl,
    mcAlgorithmFlag algorithm);

const mcMesh *mcIsosurfaceBuilder_isosurfaceFromCloud(
    mcIsosurfaceBuilder *self,
    mcScalarCloud sc,
    mcAlgorithmFlag algorithm);

#endif
