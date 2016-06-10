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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mc/algorithms/common/cube.h>
#include <mc/algorithms/patch/common.h>
#include <mc/algorithms/simple/simple_tables.h>

#include "../common/canonical_cube_orientations.h"

#define get_byte(num, byte) (((num) & (0xff << (8 * byte))) >> (8 * byte))

void mcPatch_reversePatchWindingOrder(mcPatch_Patch *patch) {
  int reversed[MC_PATCH_MAX_EDGE_INTERSECTIONS];
  assert(patch->numEdgeIntersections <= MC_PATCH_MAX_EDGE_INTERSECTIONS);
  /* Reverse the order of the array of edge intersections */
  for (int i = 0; i < patch->numEdgeIntersections; ++i) {
    reversed[i] = patch->edgeIntersections[patch->numEdgeIntersections - 1 - i];
  }
  memcpy(patch->edgeIntersections, reversed,
      sizeof(int) * patch->numEdgeIntersections);
}

void mcPatch_computePatchList(
    unsigned int cube,
    mcPatch_PatchList *patchList)
{
  unsigned int canonical, rotation;
  mcPatch_Patch *patch;
  unsigned int numPatches;
  /* Initialize the patch list */
  for (int i = 0; i < MC_PATCH_MAX_PATCHES; ++i) {
    patch = &patchList->patches[i];
    for (int j = 0; j < MC_PATCH_MAX_EDGE_INTERSECTIONS; ++j) {
      patch->edgeIntersections[j] = -1;
    }
    patch->numEdgeIntersections = 0;
  }
  /* Determine this cube's canonical orientation and the corresponding
   * rotation sequences that brings it to that orientation */
  canonical = mcCube_canonicalOrientation(cube);
  rotation = mcCube_canonicalRotation(cube);
  /* Generate patches for the canonical orientation */
  numPatches = 0;
#define make_triangle(a, b, c) \
  do { \
    patch = &patchList->patches[numPatches++]; \
    patch->edgeIntersections[0] = a; \
    patch->edgeIntersections[1] = b; \
    patch->edgeIntersections[2] = c; \
    for (int i = 3; i < MC_PATCH_MAX_EDGE_INTERSECTIONS; ++i) \
      patch->edgeIntersections[i] = -1; \
    patch->numEdgeIntersections = 3; \
  } while(0)
#define make_quad(a, b, c, d) \
  do { \
    patch = &patchList->patches[numPatches++]; \
    patch->edgeIntersections[0] = a; \
    patch->edgeIntersections[1] = b; \
    patch->edgeIntersections[2] = c; \
    patch->edgeIntersections[3] = d; \
    for (int i = 4; i < MC_PATCH_MAX_EDGE_INTERSECTIONS; ++i) \
      patch->edgeIntersections[i] = -1; \
    patch->numEdgeIntersections = 4; \
  } while(0)
#define make_pentagon(a, b, c, d, e) \
  do { \
    patch = &patchList->patches[numPatches++]; \
    patch->edgeIntersections[0] = a; \
    patch->edgeIntersections[1] = b; \
    patch->edgeIntersections[2] = c; \
    patch->edgeIntersections[3] = d; \
    patch->edgeIntersections[4] = e; \
    for (int i = 5; i < MC_PATCH_MAX_EDGE_INTERSECTIONS; ++i) \
      patch->edgeIntersections[i] = -1; \
    patch->numEdgeIntersections = 5; \
  } while(0)
#define make_hexagon(a, b, c, d, e, f) \
  do { \
    patch = &patchList->patches[numPatches++]; \
    patch->edgeIntersections[0] = a; \
    patch->edgeIntersections[1] = b; \
    patch->edgeIntersections[2] = c; \
    patch->edgeIntersections[3] = d; \
    patch->edgeIntersections[4] = e; \
    patch->edgeIntersections[5] = f; \
    for (int i = 6; i < MC_PATCH_MAX_EDGE_INTERSECTIONS; ++i) \
      patch->edgeIntersections[i] = -1; \
    patch->numEdgeIntersections = 6; \
  } while(0)
  switch (canonical) {
    case MC_CUBE_CANONICAL_ORIENTATION_0:
      /* This is a cube entirely inside or outside the isosurface, with no need
       * to generate patches */
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_1:
      /* This corresponds to a single triangle in one corner */
      make_triangle(0, 8, 3);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_2:
      /* This is the case where two samples on the same edge that are below the
       * isosurface. This makes a single quad. */
      make_quad(1, 9, 8, 3);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_3:
      /* This case has two samples on the front face which are below the
       * isosurface. Since these samples are diagonal from each other, this is
       * a case of an ambiguous face. See "The asymptotic Decider: Resolving
       * the Ambiguity in Marching Cubes," Nielson. */
      make_triangle(0, 8, 3);
      make_triangle(1, 2, 11);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_4:
      /* This case has three samples on the front face in an "L" shape that are
       * below the isosurface. The result resembles a fan or paper airplane. */
      make_pentagon(2, 11, 9, 8, 3);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_5:
      /* In this case, four samples on one face are below the isosurface. This
       * gives a quad that divides the cube squarely in half. */
      make_quad(8, 10, 11, 9);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_6:
      /* This case has two samples below the isosurface on opposite corners of
       * the cube. */
      make_triangle(1, 2, 11);
      make_triangle(4, 7, 8);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_7:
      /* This case has two samples on the same edge that are below the
       * isosurface that generate a quad, and a third sample diagonal from the
       * other two that generates a lone triangle. Since this case has a face
       * with samples diagonal from each other, we again have an ambiguous
       * face. */
      make_quad(0, 4, 7, 3);
      make_triangle(1, 2, 11);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_8:
      /* For this case, the four samples below the isosurface are arranged in
       * what appears to be a serpentine shape along the edges of the cube.
       * This is one of two cases that look like this. Only way to
       * differentiate these two cases visually is by observing the handedness
       * of the shape. This particular case has a "Z" shape when viewed from
       * the outside of the isosurface looking in. */
      make_hexagon(2, 11, 9, 4, 7, 3);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_9:
      /* This case has three samples mutually diagonal from each other that
       * generate three separate triangles. This case has a number of ambiguous
       * faces. */
      make_triangle(1, 9, 0);
      make_triangle(2, 3, 10);
      make_triangle(4, 7, 8);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_10:
      /* This case places has four samples below the isosurface arranged
       * symmetrically so that the isosurface appears to intersect the cube at
       * an angle into equal halves. */
      make_hexagon(1, 9, 4, 7, 10, 2);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_11:
      /* For this case, the four samples below the isosurface are arranged in
       * what appears to be a serpentine shape along the edges of the cube.
       * This is one of two cases that look like this. Only way to
       * differentiate these two cases visually is by observing the handedness
       * of the shape. This particular case has a "S" shape when viewed from
       * the outside of the isosurface looking in. */
      make_hexagon(0, 4, 7, 10, 11, 1);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_12:
      /* This configuration has three samples under the isosurface in an "L"
       * shape, and a fourth sample apart from the other three. */
      make_pentagon(0, 9, 11, 10, 3);
      make_triangle(4, 7, 8);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_13:
      /* This is the case with two quads facing each other. Two ambiguous edges
       * are present. */
      make_quad(3, 10, 11, 1);
      make_quad(5, 7, 8, 9);
      break;
    case MC_CUBE_CANONICAL_ORIENTATION_14:
      /* This case has four separated samples below the isosurface that
       * generate four separate triangles. */
      make_triangle(0, 1, 9);
      make_triangle(2, 3, 10);
      make_triangle(4, 7, 8);
      make_triangle(5, 11, 6);
      break;
  }

  /* Rotate the canonical patches back into our cube's orientation */
  for (int i = 0; i < MC_PATCH_MAX_PATCHES; ++i) {
    patch = &patchList->patches[i];
    if (patch->numEdgeIntersections == 0)
      break;  /* No more patches to consider */
    /* Iterate over each edge intersection */
    for (int j = 0; j < patch->numEdgeIntersections; ++j) {
      assert(patch->edgeIntersections[j] != -1);
      /* Rotate the edge intersection about the y-axis */
      for (int k = 0; k < get_byte(rotation, 2); ++k) {
        patch->edgeIntersections[j] =
          mcCube_rotateEdgeReverseY(patch->edgeIntersections[j]);
      }
      /* Rotate the edge intersection about the x-axis */
      for (int k = 0; k < get_byte(rotation, 1); ++k) {
        patch->edgeIntersections[j] =
          mcCube_rotateEdgeReverseX(patch->edgeIntersections[j]);
      }
      /* Rotate the edge intersection about the z-axis */
      for (int k = 0; k < get_byte(rotation, 0); ++k) {
        patch->edgeIntersections[j] =
          mcCube_rotateEdgeReverseZ(patch->edgeIntersections[j]);
      }
    }
    /* Cube inversion affects patch winding order */
    /* FIXME: I'm not sure why the inversion byte needs to be negated here.
     * Something is off somewhere. */
    if (!get_byte(rotation, 3)) {
      /* Reverse patch winding order to get correct front/back faces */
      mcPatch_reversePatchWindingOrder(patch);
    }
  }
}

void mcPatch_computePatchTable(mcPatch_PatchList *table) {
  /* TODO: Iterate through all voxel cube configurations */
  for (unsigned int cube = 0; cube <= 0xff; ++cube) {
    /* Compute the patch list for this cube configuration */
    mcPatch_computePatchList(cube, &table[cube]);

#ifndef NDEBUG
    /* Ensure that the edge and patch tables agree */
    for (int i = 0; i < MC_PATCH_MAX_PATCHES; ++i) {
      mcPatch_Patch *patch = &table[cube].patches[i];
      if (patch->edgeIntersections[0] == -1)
        break;  /* No more patches to consider */
      for (int j = 0; j < patch->numEdgeIntersections; ++j) {
        int found;
        unsigned int edge = patch->edgeIntersections[j];
        /* Look for this edge in the edge list */
        found = 0;
        for (int k = 0; k < MC_CUBE_NUM_EDGES; ++k) {
          if (mcSimple_edgeTable[cube].edges[k] == edge) {
            found = 1;
            break;
          }
        }
        assert(found);
      }
    }
#endif
  }
}

void mcPatch_printPatchTable(const mcPatch_PatchList *table) {
  fprintf(stdout,
      "const mcPatch_PatchList mcPatch_patchTable[] = {\n");
  for (unsigned int cube = 0; cube <= 0xff; ++cube) {
    fprintf(stdout,
        "  { .patches =\n"
        "    {\n");
    for (unsigned int i = 0; i < MC_PATCH_MAX_PATCHES; ++i) {
      fprintf(stdout,
          "      {\n"
          "        .edgeIntersections = { ");
      for (unsigned int j = 0; j < MC_PATCH_MAX_EDGE_INTERSECTIONS; ++j) {
        fprintf(stdout,
            "%2d, ",
            table[cube].patches[i].edgeIntersections[j]);
      }
      fprintf(stdout, "},\n"
          "        .numEdgeIntersections = %d,\n"
          "      },\n",
          table[cube].patches[i].numEdgeIntersections);
    }
    fprintf(stdout,
        "    },\n"
        "  },\n");
  }
  fprintf(stdout,
      "};\n");
}

int main(int argc, char **argv) {
  /* Allocate memory for the patch table */
  mcPatch_PatchList *patchTable =
    (mcPatch_PatchList*)malloc(sizeof(mcPatch_PatchList) * 256);

  /* Compute the tables */
  mcPatch_computePatchTable(patchTable);

  /* Print the tables */
  mcPatch_printPatchTable(patchTable);

  /* Free our resources */
  free(patchTable);
}
