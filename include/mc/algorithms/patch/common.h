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

#ifndef MC_ALGORITHMS_PATCH_COMMON_H_
#define MC_ALGORITHMS_PATCH_COMMON_H_

#define MC_PATCH_MAX_EDGE_INTERSECTIONS 6
#define MC_PATCH_MAX_PATCHES 4

/**
 * Represents a patch in a cube within the isosurface mesh with the given edge
 * intersections. Patches are not necessarily polygons, since there is no
 * requirement for vertices in the patch to be co-planar, but they are most
 * often nearly polygons. 
 */
typedef struct mcPatch_Patch {
  /** Edge indices for each of the edge intersections that make up this patch.
   * The maximum length of this array is MC_PATCH_MAX_EDGE_INTERSECTIONS, but
   * if the actual number of edge intersections is less than this maximum then
   * the tail of this array will be filled with -1. */
  int edgeIntersections[MC_PATCH_MAX_EDGE_INTERSECTIONS];
  /**
   * The number of edge intersections that make up this patch.
   */
  unsigned int numEdgeIntersections;
} mcPatch_Patch;

/**
 * A list of surface patches corresponding to a particular cube configuration.
 */
typedef struct mcPatch_PatchList {
  /**
   * The patches contained in this patch list. The maximum number of patches
   * for a given cube configuration is MC_PATCH_MAX_PATCHES, but if the actual
   * number of patches exceeds this number then the remaining patches in the
   * array will have no edge intersections.
   */
  mcPatch_Patch patches[MC_PATCH_MAX_PATCHES];
} mcPatch_PatchList;

#endif
