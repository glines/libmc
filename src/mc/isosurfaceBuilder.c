#include "../../include/mc/isosurfaceBuilder.h"

const mcMesh *mcIsosurfaceBuilder_buildIsosurface(
    mcIsosurfaceBuilder *self,
    mcScalarField sf,
    mcAlgorithmFlag algorithm)
{
  // TODO: Determine the isosurface generation algorithm to use
  switch (algorithm) {
    case MC_SIMPLE_MARCHING_CUBES:
      break;
    case MC_ELASTIC_SURFACE_NETS:
      break;
    case MC_SNAP_MARCHING_CUBES:
      break;
  }
}
