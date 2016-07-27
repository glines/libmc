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

#ifndef MC_ALGORITHMS_H_
#define MC_ALGORITHMS_H_

/**
 * \addtogroup libmc
 * @{
 */

/**
 * \defgroup algorithms Algorithms
 */

/**
 * \addtogroup algorithms
 * @{
 */

/** \file algorithms.h
 *
 * The declarations in this header file expose the list of isosurface
 * extraction algorithms implemented in libmc, without exposing the specifics
 * of any given algorithm.
 *
 * For additional control of any particular algorithm, include the
 * corresponding header file for that algorithm. For example, for additional
 * control over the cuberille isosurface extraction algorithm, try:
 * \code{.c}
 * #include <mc/algorithms/cuberille.h>
 * \endcode
 *
 * See the algorithm's header file's documentation for further details.
 */

/**
 * This enum type gives the different isosurface extraction algorithms
 * implemented in libmc. These flags are to be passed into the isosurface
 * extraction calls to select a specific isosurface extraction algorithm.  For
 * a detailed description on each of these algorithms, see the \ref algorithms
 * page.
 *
 * For the flags that do not mention any specific algorithms, such as those
 * that select an algorithm based on performance or quality characteristics,
 * the exact algorithm selected is subject to change. Newer versions of libmc
 * may select a different algorithm.
 */
typedef enum mcAlgorithmFlag {
  /**
   * Represents an algorithm that is not known to the current version of libmc.
   */
  MC_UNKNOWN_ALGORITHM = -1,
  /** The default algorihm used by libmc, which is currently the same algorithm
   * as chosen for MC_CPU_BALANCE_ALGORITHM. */
  MC_DEFAULT_ALGORITHM = 1,
  /** Selects a performant isosurface extraction algorithm and its
   * corresponding parameters (possibly sacrificing some quality) for execution
   * on a CPU. */
  MC_CPU_PERFORMANCE_ALGORITHM,
  /** Selects an algorithm and its parameters with a balance between
   * performance and mesh quality for execution on a CPU. */
  MC_CPU_BALANCE_ALGORITHM,
  /** Selects an algorithm and its parameters with emphasis on mesh quality
   * (possibly at the expense of some performance) for execution on a CPU. */
  MC_CPU_QUALITY_ALGORITHM,
  /** Selects a performant isosurface extraction algorithm and its
   * corresponding parameters (possibly sacrificing some quality) for execution
   * on a GPU with OpenCL. */
  MC_GPGPU_PERFORMANCE_ALGORITHM,
  /** Selects an algorithm and its parameters with emphasis on mesh quality
   * (possibly at the expense of some performance) for execution on a GPU with
   * OpenCL. */
  MC_GPGPU_QUALITY_ALGORITHM,
  /** Selects an algorithm and its parameters with a balance between
   * performance and mesh quality for execution on a GPU with OpenCL. */
  MC_GPGPU_BALANCE_ALGORITHM,
  /** Selects an algorithm with minimal working memory requirements. */
  MC_LOW_MEMORY_ALGORITHM,
  MC_SIMPLE_MARCHING_CUBES,
  MC_DUAL_MARCHING_CUBES,
  MC_ELASTIC_SURFACE_NETS,
  MC_CUBERILLE,
  MC_SNAP_MARCHING_CUBES,
  /** Marching cubes using a patch mesh instead of a triangle mesh.
   * \cite Nielson:1991 */
  MC_PATCH_MARCHING_CUBES,
  /** Marching cubes using midpoints instead of interpolated edge
   * intersections. \cite Nielson:1991 */
  MC_MIDPOINT_MARCHING_CUBES,
  /** The dual of the midpoint marching cubes mesh as described by Nielson.
   * \cite Nielson:1991 */
  MC_NIELSON_DUAL,
  /** The original marching cubes as described by Lorensen. \cite Lorensen:1987 */
  MC_ORIGINAL_MARCHING_CUBES,
  MC_TRANSVOXEL,
} mcAlgorithmFlag;

/**
 * Reads the given null-terminated string and returns the corresponding
 * isosurface extraction algorithm flag.
 *
 * \param string A null-terminated string representing an isosurface extraction
 * algorithm.
 * \return The algorithm flag corresponding to the given algorithm string.
 *
 * The identifier for the flag is used for the corresponding algorithm string.
 * For example, the original marching cubes algorithm has the algorithm string
 * "MC_ORIGINAL_MARCHING_CUBES".
 */
mcAlgorithmFlag mcAlgorithm_stringToFlag(const char *string);

/**
 * Since the generic interface for isosurface extraction does not assume any
 * particular algorithm will be used, an optional pointer to algorithm
 * parameters is needed in the event that the user wants to pass their own
 * parameters to an algorithm.
 *
 * Due to the nature of C, the type of these parameters cannot be checked at
 * compile time. It is up to the user to pass the appropriate parameter struct
 * for a given algorithm.
 */
typedef void * mcAlgorithmParams;

/**
 * This enum type lists all possible algorithm parameter structure types
 * possible within libmc. Typically, each algorithm will use its own parameter
 * type and define its own corresponding parameter structure.
 */
typedef enum mcAlgorithmParamsType {
  MC_CUBERILLE_PARAMS,
  MC_TRANSVOXEL_PARAMS,
} mcAlgorithmParamsType;

/** @} */

/** @} */

#endif
