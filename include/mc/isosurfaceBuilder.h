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

/**
 * \addtogroup libmc
 * @{
 */

/** \file mc/isosurfaceBuilder.h
 *
 * \todo Move unrelated definitions in this file to other headers.
 */

#include <stdint.h>

#include <mc/algorithms.h>
#include <mc/mesh.h>
#include <mc/scalarField.h>

/**
 * Stores a lattice of pre-gathered sample points in a regular lattice. This is
 * presumably a sampling from an infinitely dense scalar field.
 *
 * \todo Support for scalar lattices as input should be implemented.
 * \todo Different filtering schemes for scalar lattice input, such as nearest
 * neighbor, trilinear interpolation, and higher order interpolation, should be
 * supported.
 */
typedef struct mcScalarLattice {
  float *lattice;
  unsigned int size[3];
  float delta[3];
} mcScalarLattice;

/**
 * A single sample point within a sample cloud.
 *
 * \sa mcScalarCloud
 */
typedef struct mcScalarPoint {
  float pos[3];
  float value;
} mcScalarPoint;

/**
 * A collection of scalar samples with arbitrary positions. This data structure
 * is much more challenging to work with than a lattice.
 */
typedef struct mcScalarCloud {
  mcScalarPoint *cloud;
  unsigned int size;
} mcScalarCloud;

/**
 * \internal
 * The internal data structures of the isosurface builder. In particular, this
 * includes memory for storing the generated meshes.
 * \endinternal
 */
typedef struct mcIsosurfaceBuilderInternal mcIsosurfaceBuilderInternal;

/**
 * Builds isosurface meshes from any given representation of the isosurface.
 * The isosurface builder provides the primary interface for using libmc. This
 * interface allows one to build meshes using any algorithm that libmc
 * implements.
 *
 * As a side note, this interface is inspired by the Importer interface from
 * assimp.
 */
typedef struct mcIsosurfaceBuilder {
  mcIsosurfaceBuilderInternal *internal;
} mcIsosurfaceBuilder;

/**
 * Initializes the isosurface builder structure. In particular, this allocates
 * memory for the structures that store mesh data.
 *
 * \param self The isosurface builder structure to initialize.
 */
void mcIsosurfaceBuilder_init(
    mcIsosurfaceBuilder *self);

/**
 * Destroys the isosurface builder structure. This frees any memory that was
 * allocated for meshes generated through this isosurface builder.
 *
 * \param self The mcMesh structure to destroy.
 */
void mcIsosurfaceBuilder_destroy(
    mcIsosurfaceBuilder *self);

/**
 * Builds an isosurface using the given parameters and returns the result as a
 * constant pointer to a mesh structure. Any number of algorithms can be used
 * by passing different enum values to the \p algorithm parameter.
 *
 * \param self The isosurface builder object to do the building.
 * \param sf The scalar field function defining the implicit isosurface.
 * \param algorithm Flag representing the isosurface extraction algorithm to
 * use.
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
 * \return A mesh structure representing the isosurface that was extracted.
 *
 * The memory of the returned mesh structure is owned by the isosurface builder
 * structure itself.
 *
 * This is actually a convenience wrapper for
 * mcIsosurfaceBuilder_isosurfaceFromFieldWithArgs() that simply passes a
 * scalar field without args to the other method.
 *
 * \sa mcIsosurfaceBuilder_isosurfaceFromFieldWithArgs()
 */
const mcMesh *mcIsosurfaceBuilder_isosurfaceFromField(
    mcIsosurfaceBuilder *self,
    mcScalarField sf,
    mcAlgorithmFlag algorithm,
    unsigned int x_res, unsigned int y_res, unsigned int z_res,
    const mcVec3 *min, const mcVec3 *max);

/**
 * Builds an isosurface using the given parameters and returns the result as a
 * constant pointer to a mesh structure. Any number of algorithms can be used
 * by passing different enum values to the \p algorithm parameter. Auxillary
 * arguments for the scalar field function can be passed in the \p args
 * parameter for additional flexibility.
 *
 * \param self The isosurface builder object to do the building.
 * \param sf The scalar field function defining the implicit isosurface.
 * \param args Auxiliary arguments to the scalar field function, which
 * facilitate more flexible "functor" implementations of the scalar field
 * function.
 * \param algorithm Flag representing the isosurface extraction algorithm to
 * use.
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
 * \return A mesh structure representing the isosurface that was extracted.
 *
 * The memory of the returned mesh structure is owned by the isosurface builder
 * structure itself.
 *
 * \sa mcIsosurfaceBuilder_isosurfaceFromField()
 */
const mcMesh *mcIsosurfaceBuilder_isosurfaceFromFieldWithArgs(
    mcIsosurfaceBuilder *self,
    mcScalarFieldWithArgs sf,
    const void *args,
    mcAlgorithmFlag algorithm,
    unsigned int x_res, unsigned int y_res, unsigned int z_res,
    const mcVec3 *min, const mcVec3 *max);

/**
 * Builds an isosurface mesh from a pre-sampled lattice. This method of
 * building isosurface meshes requires a pre-sampled lattice, which has the
 * potential to use more memory than passing a callback function. This might be
 * useful for the cases where the scalar field itself is defined by empirical
 * data gathered in advance, such as with a CT scan. 
 *
 * \param self The isosurface builder object to do the building.
 * \param sl A pre-sampled lattice of sample values which together define the
 * isosurface.
 * \param algorithm Flag representing the isosurface extraction algorithm to
 * use.
 * \return A mesh structure representing the isosurface that was extracted.
 */
const mcMesh *mcIsosurfaceBuilder_isosurfaceFromLattice(
    mcIsosurfaceBuilder *self,
    mcScalarLattice sl,
    mcAlgorithmFlag algorithm);

/**
 * Builds an isosurface mesh from a pre-sampled cloud of sample points. The
 * samples are not constrained to be part of a lattice structure, which is
 * a typical output format for many types of surface scans.
 *
 * \param self The isosurface builder doing the building.
 * \param sc The scalar cloud from which to gather data hinting at the
 * isosurface.
 * \param algorithm The isosurface extraction alogrithm to be used.
 * \return A mesh structure representing the isosurface that was extracted.
 *
 * This type of data may need to be converted to a lattice of samples in order
 * to be used with certain algorithms, such as marching cubes. Other
 * algorithms, such as many Voronoi based methods, can use this type of data
 * as is. As such, the choice of algorithm is critical for point cloud data.
 */
const mcMesh *mcIsosurfaceBuilder_isosurfaceFromCloud(
    mcIsosurfaceBuilder *self,
    mcScalarCloud sc,
    mcAlgorithmFlag algorithm);

/** @} */

#endif
