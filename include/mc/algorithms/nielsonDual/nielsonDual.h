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

#ifndef MC_ALGORITHMS_NIELSON_DUAL_NIELSON_DUAL_H_
#define MC_ALGORITHMS_NIELSON_DUAL_NIELSON_DUAL_H_

/**
 * \addtogroup libmc
 * @{
 */

/**
 * \addtogroup algorithms
 * @{
 */

/**
 * \addtogroup nielsonDual
 * @{
 */

/** \file mc/algorithms/nielsonDual/nielsonDual.h
 *
 * This header file contains all of the declarations needed to use the dual of
 * the midpoint marching cubes as described by Nielson in \cite Nielson:1991.
 */

#include <mc/isosurfaceBuilder.h>

/**
 * Builds an isosurface mesh using the dual of the halfway marching cubes mesh,
 * as described by Nielson in \cite Nielson:2004.
 *
 * \param sf The scalar field function defining the implicit isosurface.
 * \param args Auxiliary arguments to the scalar field function, which
 * facilitate more flexible "functor" implementations of the scalar field
 * function.
 * \param x_res The number of samples to take in the sample lattice parallel to
 * the x-axis.
 * \param y_res The number of samples to take in the sample lattice parallel to
 * the y-axis.
 * \param z_res The number of samples to take in the sample lattice parallel to
 * the z-axis.
 * \param min The absolute position where the sample lattice begins and the
 * first sample is to be taken.
 * \param max The absolute position where the sample lattice ends and the last
 * sample is to be taken.
 * \param mesh The mesh structure in which the isosurface mesh is to be built.
 */
void mcNielsonDual_isosurfaceFromField(
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int x_res, unsigned int y_res, unsigned int z_res,
    const mcVec3 *min, const mcVec3 *max,
    mcMesh *mesh);

/** @} */

/** @} */

/** @} */

#endif
