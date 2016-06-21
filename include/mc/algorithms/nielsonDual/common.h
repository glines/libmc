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

#ifndef MC_ALGORITHMS_NIELSON_DUAL_COMMON_H_
#define MC_ALGORITHMS_NIELSON_DUAL_COMMON_H_

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

#include <mc/algorithms/common/cube.h>
#include <mc/vector.h>

/**
 * The maximum number of vertices that can be generated for a single cube
 * configuration. This number can be easily verified by looking at the diagrams
 * in \cite Nielson:2004.
 */
#define MC_NIELSON_DUAL_MAX_VERTICES 4

/**
 * This structure represents a vertex that should be generated for a given cube
 * configuration in the MC-Dual algorithm. The edge intersections and cube
 * connectivity should completely describe the vertex. This information is used
 * to generate additional tables.
 *
 * \todo We can use a much more compact representation for mcNielsonDualVertex,
 * which might provide some performance increase. Although at the moment I
 * think this structure is only being used to generate tables.
 */
typedef struct mcNielsonDualVertex {
  /* FIXME: The maximum number of edge intersections is less than this */
  int edgeIntersections[MC_CUBE_NUM_EDGES];
  /* NOTE: The connectivity lists faces, but as some faces might intersect with
   * more than one vertex edge this connectivity must be disambiguated by
   * looking for common edge intersections at runtime.
   */
  int connectivity[MC_CUBE_NUM_FACES];
} mcNielsonDualVertex;

/**
 * List of all MC-Dual vertices generated for a particular sample voxel cube
 * configuration.
 *
 * \sa mcNielsonDualVertex
 */
typedef struct mcNielsonDualVertexList {
  mcNielsonDualVertex vertices[MC_NIELSON_DUAL_MAX_VERTICES];
} mcNielsonDualVertexList;

/**
 * MC-Dual vertex with pre-computed vertex positions and normals.
 *
 * These positions and normals can be pre-computed because of the assumption
 * made that all sample lattice edge intersections are made at the midpoint of
 * the edge. This approximation is not always perfect, but the results are more
 * than acceptable for many applications. These positions and normals are said
 * to be "pre-cooked."
 *
 * \sa mcNielsonDualCookedVertexList
 */
typedef struct mcNielsonDualCookedVertex {
  mcVec3 pos, norm;
} mcNielsonDualCookedVertex;

/**
 * List of MC-Dual vertices for a particular cube configuration with
 * pre-computed vertex positions and normals.
 *
 * \sa mcNielsonDualCookedVertex
 */
typedef struct mcNielsonDualCookedVertexList {
  mcNielsonDualCookedVertex vertices[MC_NIELSON_DUAL_MAX_VERTICES];
  int numVertices;
} mcNielsonDualCookedVertexList;

/** @} */

/** @} */

/** @} */

#endif
