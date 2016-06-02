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
