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

#ifndef MC_ALGORITHMS_CUBERILLE_CUBERILLE_H_
#define MC_ALGORITHMS_CUBERILLE_CUBERILLE_H_

#include <mc/isosurfaceBuilder.h>

/**
 * A parameter structure that can optionally be passed into the cuberille
 * isosurface extraction algorithm.
 */
typedef struct mcCuberilleParams {
  /** This field \em must be set to the value MC_CUBERILLE_PARAMS or a runtime
   * error will occur. */
  mcAlgorithmParamsType type;
} mcCuberilleParams;

/**
 * Initializes the given \p params structure with the default parameters for
 * the cuberille isosurface extraction algorithm.
 *
 * The specific values of these default parameters depends on the version of
 * the libmc library used.
 */
void mcCuberilleParams_default(mcCuberilleParams *params);

/**
 * This routine implements the "cuberille" isosurface extraction algorithm as
 * described in FIXME.  This algorithm is the precursor to elastic surface nets
 * and other "dual" methods.
 *
 * \todo Find a good reference for the cuberille isosurface extraction
 * algorithm.
 */
void mcCuberille_isosurfaceFromField(
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int res_x, unsigned int res_y, unsigned int res_z,
    const mcVec3 *min, const mcVec3 *max,
    mcCuberilleParams *params,
    mcMesh *mesh);

#endif
