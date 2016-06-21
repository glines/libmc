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
#include <stdlib.h>
#include <string.h>

#include <mc/algorithms/cuberille.h>
#include <mc/algorithms/dualMarchingCubes.h>
#include <mc/algorithms/elasticSurfaceNet.h>
#include <mc/algorithms/nielsonDual.h>
#include <mc/algorithms/patch.h>
#include <mc/algorithms/simple.h>
#include <mc/isosurfaceBuilder.h>
#include <mc/mesh.h>

/**
 * \internal
 * The internal data structures for the isosurface builder. In particular, this
 * includes the storage for all of the meshes generated and not yet destroyed.
 * \endinternal
 */
struct mcIsosurfaceBuilderInternal {
  /** Stored meshes that were built or are currently being built by this
   * isosurface builder. */
  mcMesh *meshes;
  /** The size of the meshes member, i.e. the maximum number of meshes that
   * this isosurface builder structure can refer to without growing the meshes
   * data member. */
  unsigned int meshesSize;
  /** The number of meshes that this isosurface builder currently holds. */
  unsigned int numMeshes;
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

/**
 * This method allows us to pass an mcScalarField (without arguments) as an
 * mcScalarFieldWithArgs.
 */
float mcIsosurfaceBuilder_scalarFieldNullArgs(
    float x, float y, float z, mcScalarField sf)
{
  return sf(x, y, z);
}

const mcMesh *mcIsosurfaceBuilder_isosurfaceFromField(
    mcIsosurfaceBuilder *self,
    mcScalarField sf,
    mcAlgorithmFlag algorithm,
    unsigned int x_res, unsigned int y_res, unsigned int z_res,
    const mcVec3 *min, const mcVec3 *max)
{
  return mcIsosurfaceBuilder_isosurfaceFromFieldWithArgs(
      self,
      (mcScalarFieldWithArgs)mcIsosurfaceBuilder_scalarFieldNullArgs,
      sf,
      algorithm,
      x_res, y_res, z_res,
      min, max);
}

const mcMesh *mcIsosurfaceBuilder_isosurfaceFromFieldWithArgs(
    mcIsosurfaceBuilder *self,
    mcScalarFieldWithArgs sf,
    const void *args,
    mcAlgorithmFlag algorithm,
    unsigned int x_res, unsigned int y_res, unsigned int z_res,
    const mcVec3 *min, const mcVec3 *max)
{
  /* Initialize a mesh */
  if (self->internal->numMeshes >= self->internal->meshesSize) {
    mcIsosurfaceBuilder_growMeshes(self);
  }
  mcMesh *mesh = &self->internal->meshes[self->internal->numMeshes++];
  mcMesh_init(mesh);
  /* TODO: Determine the isosurface generation algorithm to use */
  switch (algorithm) {
    case MC_CPU_PERFORMANCE_ALGORITHM:
      /* TODO */
      assert(0);
      break;
    case MC_DEFAULT_ALGORITHM:
    case MC_CPU_BALANCE_ALGORITHM:
      /* TODO */
      assert(0);
      break;
    case MC_CPU_QUALITY_ALGORITHM:
      /* TODO */
      assert(0);
      break;
    case MC_GPGPU_PERFORMANCE_ALGORITHM:
      /* TODO */
      assert(0);
      break;
    case MC_GPGPU_BALANCE_ALGORITHM:
      /* TODO */
      assert(0);
      break;
    case MC_GPGPU_QUALITY_ALGORITHM:
      /* TODO */
      assert(0);
      break;
    case MC_LOW_MEMORY_ALGORITHM:
      /* TODO */
      assert(0);
      break;
    case MC_MIDPOINT_MARCHING_CUBES:
      /* TODO */
      assert(0);
      break;
    case MC_SIMPLE_MARCHING_CUBES:
    case MC_ORIGINAL_MARCHING_CUBES:
      mcSimple_isosurfaceFromField(
          sf, args,
          x_res, y_res, z_res,
          min, max,
          mesh);
      break;
    case MC_DUAL_MARCHING_CUBES:
      mcDualMarchingCubes_isosurfaceFromField(
          sf, args,
          x_res, y_res, z_res,
          min, max,
          mesh);
      break;
    case MC_ELASTIC_SURFACE_NETS:
      mcElasticSurfaceNet_isosurfaceFromField(
          sf, args,
          x_res, y_res, z_res,
          min, max,
          mesh);
      break;
    case MC_CUBERILLE:
      mcCuberille_isosurfaceFromField(
          sf, args,
          x_res, y_res, z_res,
          min, max,
          NULL,  /* TODO: Pass the params into this function */
          mesh);
      break;
    case MC_SNAP_MARCHING_CUBES:
      break;
    case MC_PATCH_MARCHING_CUBES:
      mcPatch_isosurfaceFromField(
          sf, args,
          x_res, y_res, z_res,
          min, max,
          mesh);
      break;
    case MC_NIELSON_DUAL:
      mcNielsonDual_isosurfaceFromField(
          sf, args,
          x_res, y_res, z_res,
          min, max,
          mesh);
      break;
  }
  return mesh;
}

const mcMesh *mcIsosurfaceBuilder_isosurfaceFromLattice(
    mcIsosurfaceBuilder *self,
    mcScalarLattice sl,
    mcAlgorithmFlag algorithm)
{
  /* TODO */
  return NULL;
}

const mcMesh *mcIsosurfaceBuilder_isosurfaceFromCloud(
    mcIsosurfaceBuilder *self,
    mcScalarCloud sc,
    mcAlgorithmFlag algorithm)
{
  /* TODO */
  return NULL;
}
