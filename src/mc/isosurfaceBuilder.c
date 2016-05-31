#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <mc/isosurfaceBuilder.h>
#include <mc/mesh.h>

#include "algorithms/simple.h"

struct mcIsosurfaceBuilderInternal {
  mcMesh *meshes;
  unsigned int meshesSize, numMeshes;
};

void mcIsosurfaceBuilder_init(
    mcIsosurfaceBuilder *self)
{
  const unsigned int INIT_NUM_MESHES = 4;

  /* Allocate memory for the internal data structures */
  self->internal =
    (mcIsosurfaceBuilderInternal*)malloc(sizeof(mcIsosurfaceBuilderInternal));
  self->internal->meshes =
    (mcMesh*)malloc(sizeof(mcMesh) * INIT_NUM_MESHES);
  self->internal->meshesSize = INIT_NUM_MESHES;
  self->internal->numMeshes = 0;
}

void mcIsosurfaceBuilder_destroy(
    mcIsosurfaceBuilder *self)
{
  /* Destroy all initialized meshes */
  for (int i = 0; i < self->internal->numMeshes; ++i) {
    mcMesh_destroy(&self->internal->meshes[i]);
  }

  /* Free internal memory */
  free(self->internal->meshes);
  free(self->internal);
}

/**
 * Doubles the size of the internal list of meshes.
 */
void mcIsosurfaceBuilder_growMeshes(
    mcIsosurfaceBuilder *self)
{
  mcMesh *newMeshes =
    (mcMesh*)malloc(sizeof(mcMesh) * self->internal->meshesSize * 2);
  memcpy(
      newMeshes,
      self->internal->meshes,
      sizeof(mcMesh) * self->internal->meshesSize);
  free(self->internal->meshes);
  self->internal->meshes = newMeshes;
  self->internal->meshesSize *= 2;
  assert(self->internal->numMeshes < self->internal->meshesSize);
}

const mcMesh *mcIsosurfaceBuilder_isosurfaceFromField(
    mcIsosurfaceBuilder *self,
    mcScalarField sf,
    mcAlgorithmFlag algorithm)
{
  /* Initialize a mesh */
  if (self->internal->numMeshes >= self->internal->meshesSize) {
    mcIsosurfaceBuilder_growMeshes(self);
  }
  mcMesh *mesh = &self->internal->meshes[self->internal->numMeshes++];
  mcMesh_init(mesh);
  /* TODO: Determine the isosurface generation algorithm to use */
  switch (algorithm) {
    case MC_SIMPLE_MARCHING_CUBES:
      mcSimple_isosurfaceFromField(sf, mesh);
      break;
    case MC_ELASTIC_SURFACE_NETS:
      break;
    case MC_SNAP_MARCHING_CUBES:
      break;
  }
}

const mcMesh *mcIsosurfaceBuilder_isosurfaceFromLattice(
    mcIsosurfaceBuilder *self,
    mcScalarLattice sl,
    mcAlgorithmFlag algorithm)
{
}

const mcMesh *mcIsosurfaceBuilder_isosurfaceFromCloud(
    mcIsosurfaceBuilder *self,
    mcScalarCloud sc,
    mcAlgorithmFlag algorithm)
{
}
