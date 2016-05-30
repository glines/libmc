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

typedef struct mcIsosurfaceBuilder {
} mcIsosurfaceBuilder;

const mcMesh *mcIsosurfaceBuilder_buildIsosurface(
    mcIsosurfaceBuilder *self,
    mcScalarField sf,
    mcAlgorithmFlag algorithm);

#endif
