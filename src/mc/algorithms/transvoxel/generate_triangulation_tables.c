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

#include "transvoxel_canonical_cells.h"

#include <mc/algorithms/transvoxel/canonical.h>
#include <mc/algorithms/transvoxel/common.h>
#include <mc/algorithms/transvoxel/transform.h>

#define get_byte(num, byte) (((num) & (0xff << (8 * byte))) >> (8 * byte))

void mcTransvoxel_computeRegularCellTriangulationTable(
    mcTransvoxel_RegularCellTriangleList *table)
{
  /* The triangulation table used in the Transvoxel algorithm solves the face
   * ambiguity problem in marching cubes by not inverting the cases with
   * ambiguous faces. This results in a few additional equivalence classes
   * which arbitrarily decide the triangulation for these otherwise ambiguous
   * cases. */
  /* Iterate over all regular cells */
  for (int cell = 0; cell <= 0xff; ++cell) {
    mcTransvoxel_RegularCellTriangleList *list = &table[cell];
    int numTriangles = 0;
    memset(list->triangles, -1, sizeof(list->triangles));
    /* Determine the canonical orientation for this regular cell and the
     * sequence of operations needed to get to that orientation */
    int canonical = mcTransvoxel_canonicalRegularCell(cell);
    int sequence = mcTransvoxel_canonicalRegularCellSequence(cell);
    /* Generate triangles for the canonical regular cell */
#define MAKE_TRIANGLE(a, b, c) \
    do { \
      mcTransvoxel_Triangle *triangle = &list->triangles[numTriangles++]; \
      triangle->edgeIntersections[0] = a; \
      triangle->edgeIntersections[1] = b; \
      triangle->edgeIntersections[2] = c; \
    } while (0)
    switch (canonical) {
      case MC_TRANSVOXEL_CANONICAL_REGULAR_CELL_0:
        /* This is the trivial case with all samples above the isosurface. No
         * triangles are needed. */
        break;
      case MC_TRANSVOXEL_CANONICAL_REGULAR_CELL_1:
        break;
    }
  }
}

void mcTransvoxel_computeTransitionCellTriangulationTable(
    mcTransvoxel_TransitionCellTriangleList *table)
{
  /* Iterate over all transition cells */
  for (int cell = 0; cell <= 0x1ff; ++cell) {
    mcTransvoxel_TransitionCellTriangleList *list = &table[cell];
    int numTriangles = 0;
    memset(list->triangles, -1, sizeof(list->triangles));
    /* Determine the canonical orientation for this transition cell and the
     * sequence of operations needed to get to that orientation */
    int canonical = mcTransvoxel_canonicalTransitionCell(cell);
    int sequence = mcTransvoxel_canonicalTransitionCellSequence(cell);
    /* Generate triangles for the canonical transition cell */
#define MAKE_TRIANGLE(a, b, c) \
    do { \
      mcTransvoxel_Triangle *triangle = &list->triangles[numTriangles++]; \
      triangle->edgeIntersections[0] = a; \
      triangle->edgeIntersections[1] = b; \
      triangle->edgeIntersections[2] = c; \
    } while (0)
    switch (canonical) {
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_0:
        /* This is the trivial case with all samples above the isosurface. No
         * triangles are needed. */
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_1:
        /* This case has a single sample below the isosurface on the corner of
         * the high resolution face. This sample is also present on the low
         * resolution face, and a quad is generated as a result.
         *
         * This corresponds to equivalence class #1 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 4, 3);
        MAKE_TRIANGLE(3, 4, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_2:
        /* This case has a single sample below the isosurface on the outer edge
         * of the high resolution face. Since none of the samples on the
         * corners are below the isosurface, the entire low resolution face is
         * above the isosurface. A single triangle is generated around the
         * sample below the isosurface.
         *
         * This corresponds to equivalence class #2 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(4, 5, 11);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_3:
        /* This case has two adjacent samples below the isosurface on the high
         * resolution face, which make an L with the corresponding sample on
         * the low resolution face. Three triangles are generated
         *
         * This corresponds to equivalence class #4 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 5, 11);
        MAKE_TRIANGLE(0, 11, 3);
        MAKE_TRIANGLE(3, 11, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_4:
        /*
         * This case has two samples on corners of the high resolution face
         * below the isosurface, which correspond to two samples on the low
         * resolution face below the isosurface. The isosurface makes a scoop
         * shape, which can be triangulated as a quad with an additional
         * triangle on each side for a total of four triangles.
         *
         * This corresponds to equivalence class #5 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 5);
        MAKE_TRIANGLE(1, 5, 3);
        MAKE_TRIANGLE(3, 5, 4);
        MAKE_TRIANGLE(3, 4, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_5:
        /* This case has three samples below the isosurface all in a row on one
         * edge of the high resolution face, and two corresponding samples
         * below the isosurface on the low resolution face. This forms a
         * relatively flat trapezoid shape with three triangles.
         *
         * This corresponds to equivalence class #12 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 11);
        MAKE_TRIANGLE(1, 11, 3);
        MAKE_TRIANGLE(3, 11, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_6:
        /* This case has two samples below the isosurface on the high
         * resolution face, and no samples below the isosurface on the high
         * resolution face. The samples below the isosurface are diagonal from
         * each other, but they are not close enough to form polygons that
         * touch. This generates two separate triangles on the high resolution
         * face, one triangle for each sample below the isosurface.
         *
         * This corresponds to equivalence class #9 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(4, 5, 11);
        MAKE_TRIANGLE(6, 13, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_7:
        /* This case has three samples below the isosurface on one corner of
         * the high resolution face, and a corresponding sample below the
         * isosurface on the low resolution face. This results in four edge
         * intersections on the high resolution face that appear to be roughly
         * in a straight line. The resulting polygon is roughly a trapezoid
         * shape made from four triangles. More than one triangulation is
         * possible here.
         *
         * This corresponds to equivalence class #13 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 5, 11);
        MAKE_TRIANGLE(0, 11, 3);
        MAKE_TRIANGLE(3, 11, 6);
        MAKE_TRIANGLE(3, 6, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_8:
        /* This case has one sample below the isosurface in a corner of the
         * high resolution face, and a second sample below the isosurface on
         * the far edge of the high resolution face. There is one corresponding
         * sample below the isosurface on the low resolution face. This
         * produces a quad near the corner of the high resolution face, as well
         * as a lone triangle on the high resolution face.
         *
         * This corresponds to equivalence class #7 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 1, 5);
        MAKE_TRIANGLE(1, 12, 5);
        MAKE_TRIANGLE(6, 13, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_9:
        /* This case has two adjacent samples below the isosurface on the high
         * resolution face as well as a third sample below the isosurface in a
         * corner of the high resolution face. Two correspnoding samples are
         * below the isosurface on the low resolution face. This results in 5
         * triangles in an unbalanced scoop shape.
         *
         * This corresponds to equivalence class #16 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 5);
        MAKE_TRIANGLE(1, 5, 4);
        MAKE_TRIANGLE(1, 4, 3);
        MAKE_TRIANGLE(3, 4, 6);
        MAKE_TRIANGLE(3, 6, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_10:
        /* This is a case with two adjacent samples below the isosurface on the
         * high resolution face as well as a third sample below the isosurface
         * on one edge of the high resolution face. The low resolution face has
         * one corresponding sample below the isosurface. This produces a
         * polygon composed of three triangles and a second lone polygon
         * composed of a single triangle.
         *
         * This corresponds to equivalence class #15 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 11);
        MAKE_TRIANGLE(0, 1, 11);
        MAKE_TRIANGLE(0, 11, 4);
        MAKE_TRIANGLE(6, 13, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_11:
        /* This case has four samples below the isosurface on the high
         * resolution cell that form an L shape. Two corresponding samples are
         * below the isosurface on the low resolution face. This generates four
         * triangles in a swooping shape.
         *
         * This corresponds to equivalence class #28 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 11);
        MAKE_TRIANGLE(1, 11, 3);
        MAKE_TRIANGLE(3, 11, 6);
        MAKE_TRIANGLE(3, 6, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_12:
        /* This case has a single sample below the isosurface in the center of
         * the high resolution face. There are no correspnoding samples below
         * the isosurface on the low resolution face. This results in a single
         * quad in the center of the high resolution face, which can be
         * triangulated in two ways.
         *
         * This corresponds to equivalence class #3 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(6, 11, 7);
        MAKE_TRIANGLE(7, 14, 6);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_13:
        /* This case has a sample below the isosurface in the center of the
         * high resolution face, as well as a sample below the isosurface a
         * corner of the high resolution face. There is one corresponding
         * sample below the isosurface face on the low resolution face. This
         * results in a quad in the center of the high resolution face, as well
         * as a separate quad on the lateral edge below the isosurface.
         *
         * This corresponds to equivalence class #7 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(6, 11, 7);
        MAKE_TRIANGLE(7, 14, 6);
        MAKE_TRIANGLE(0, 4, 3);
        MAKE_TRIANGLE(3, 4, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_14:
        /* This case has a sample below the isosurface in the center of the
         * high resolution face, as well as a sample below the isosurface on an
         * edge of the high resolution face. There are no samples below the
         * isosurface on the low resolution face. This results in a quad on the
         * edge of the high resolution face with a triangle to cap it off in a
         * pentagon or "doll house" shape.
         *
         * This corresponds to equivalence class #10 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(4, 5, 6);
        MAKE_TRIANGLE(5, 7, 6);
        MAKE_TRIANGLE(6, 7, 14);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_15:
        /* This case has three samples below the isosurface that form a small L
         * shape with the start of the L at the center of the high resolution
         * face. There is one corresponding sample below the isosurface on the
         * low resolution face.
         *
         * This corresponds to equivalence class #14 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 5, 7);
        MAKE_TRIANGLE(0, 7, 14);
        MAKE_TRIANGLE(0, 14, 3);
        MAKE_TRIANGLE(3, 14, 6);
        MAKE_TRIANGLE(3, 6, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_16:
        /* This case has a sample below the isosurface in the center of the
         * high resolution face, as well as two samples in corners of the high
         * resolution face. This results in two corresponding samples below the
         * isosurface on the low resolution face. A quad in the center of the
         * high resolution face is generated along with a separate scoop shape.
         *
         * This corresponds to equivalence class #19 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(6, 11, 7);
        MAKE_TRIANGLE(7, 14, 6);
        MAKE_TRIANGLE(1, 12, 5);
        MAKE_TRIANGLE(1, 5, 4);
        MAKE_TRIANGLE(1, 4, 3);
        MAKE_TRIANGLE(3, 4, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_17:
        /* This case has four samples below the isosurface in a T shape on the
         * high resolution face, with two of the samples on corners of the
         * face. This results in two corresponding samples below the isosurface
         * on the low resolution face. The five triangles generated form a
         * mound shape.
         *
         * This corresponds to equivalence class #29 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 7);
        MAKE_TRIANGLE(1, 7, 14);
        MAKE_TRIANGLE(1, 14, 3);
        MAKE_TRIANGLE(3, 14, 6);
        MAKE_TRIANGLE(3, 6, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_18:
        /* This case has three samples below the isosurface in an L shape on
         * the high resolution face, with none of these samples on a corner of
         * the face. There are no samples below the isosurface on the low
         * resolution face. The four triangles generated form a hexagon cap on
         * the high resolution face.
         *
         * This corresponds to equivalence class #25 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(4, 5, 7);
        MAKE_TRIANGLE(4, 7, 10);
        MAKE_TRIANGLE(7, 14, 10);
        MAKE_TRIANGLE(10, 14, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_19:
        /* This case has four samples below the isosurface that form a square
         * shape in one corner of the high resolution face. There is one
         * corresponding sample below the isosurface on the low resolution
         * face. The triangles generated form a convex hump. A couple different
         * triangulations are possible.
         *
         * This corresponds to equivalence class #32 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 5, 7);
        MAKE_TRIANGLE(0, 7, 3);
        MAKE_TRIANGLE(3, 7, 14);
        MAKE_TRIANGLE(3, 14, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_20:
        /* This case has three samples below the isosurface on the high
         * resolution face: the sample in the center, a sample on an edge, and
         * a sample on a corner. There is one corresponding sample below the
         * isosurface on the low resolution face. The sample on the edge and
         * the sample in the center generate a pentagon or "doll house" shape
         * with three triangles. The samples in the corners generate a quad.
         *
         * This corresponds to equivalence class #22 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(7, 14, 11);
        MAKE_TRIANGLE(10, 11, 14);
        MAKE_TRIANGLE(10, 14, 13);
        MAKE_TRIANGLE(1, 12, 5);
        MAKE_TRIANGLE(0, 1, 5);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_21:
        /* This case has three samples in an L shape below the isosurface on
         * the high resolution face, with one of these samples being in the
         * center and one of these samples on a corner. A fourth sample is on
         * another corner. There are thwo corresponding samples below the
         * isosurface on the low resolution face. The seven triangles generated
         * by this cell form a snaking surface.
         *
         * This corresponds to equivalence class #35 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 5);
        MAKE_TRIANGLE(1, 5, 4);
        MAKE_TRIANGLE(1, 4, 11);
        MAKE_TRIANGLE(1, 11, 7);
        MAKE_TRIANGLE(1, 7, 14);
        MAKE_TRIANGLE(1, 14, 3);
        MAKE_TRIANGLE(3, 14, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_22:
        /* This case has four samples in a Z shape below the isosurface on the
         * high resolution face, and one sample below the isosurface on the
         * corresponding corner of the low resolution face. The six triangles
         * generated by this case form a complicated shape that is mostly
         * convex, surrounding the samples below the isosurface.
         *
         * This corresponds to equivalence class #34 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 7);
        MAKE_TRIANGLE(1, 7, 14);
        MAKE_TRIANGLE(1, 14, 13);
        MAKE_TRIANGLE(1, 13, 0);
        MAKE_TRIANGLE(0, 13, 10);
        MAKE_TRIANGLE(0, 10, 4);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_23:
        /* This case has two samples below the isosurface on opposite edges of
         * the high resolution face. There are no samples below the isosurface
         * on the low resolution face. This generates two separate triangle
         * caps on the high resolution face.
         *
         * This corresponds to equivalence class #11 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(7, 12, 15);
        MAKE_TRIANGLE(6, 13, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_24:
        /* This case has two samples below the isosurface adjacent to each
         * other in one corner of the high resolution face, and a third sample
         * below the isosurface on an edge across from those samples. A
         * corresponding sample below the isosurface is in one corner of the
         * low resolution face. Three triangles are generated for the samples
         * in the corner, and a fourth triangle is generated as a cap for the
         * sample on the edge.
         *
         * This corresponds to equivalence class #17 in \cite Lengyel:2010, not
         * to be confused with equivalence class #15.
         */
        MAKE_TRIANGLE(0, 4, 6);
        MAKE_TRIANGLE(0, 6, 3);
        MAKE_TRIANGLE(3, 6, 13);
        MAKE_TRIANGLE(7, 12, 15);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_25:
        /* This case has three samples below the isosurface on the high
         * resolution face, with each of these samples on an edge of the high
         * resolution face. There are no samples below the isosurface on the
         * low resolution face. This generates three separate triangles to cap
         * the mesh on the high resolution face.
         *
         * This corresponds to equivalence class #26 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(4, 5, 11);
        MAKE_TRIANGLE(6, 13, 10);
        MAKE_TRIANGLE(7, 12, 15);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_26:
        /* This case has three samples below the isosurface on the high
         * resolution face in one corner, and a fourth sample below the
         * isosurface on the high resolution face on an edge. There is a
         * corresponding sample below the isosurface on the low resolution
         * face. This generates a nearly trapezoidal shape with four triangles
         * in the corner, and a lone triangle to cap the mesh on the edge.
         *
         * This corresponds to equivalence class #33 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 5, 11);
        MAKE_TRIANGLE(0, 11, 3);
        MAKE_TRIANGLE(3, 11, 6);
        MAKE_TRIANGLE(3, 6, 13);
        MAKE_TRIANGLE(7, 12, 15);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_27:
        /* This case has four samples below the isosurface on the high
         * resolution face. Two samples are below the isosurface on the low
         * resolution face. The triangles generated by this case form a deep
         * scoop shape.
         *
         * This corresponds to equivalence class #42 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 7);
        MAKE_TRIANGLE(1, 7, 5);
        MAKE_TRIANGLE(1, 5, 3);
        MAKE_TRIANGLE(3, 5, 4);
        MAKE_TRIANGLE(3, 4, 6);
        MAKE_TRIANGLE(3, 6, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_28:
        /* This case has three samples below the isosurface in a row along the
         * center of the high resolution face. There are no samples below the
         * isosurface on the low resolution face. This generates a rectangle
         * shape composed of two quads that caps the isosurface mesh.
         *
         * This corresponds to equivalence class #27 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(10, 11, 14);
        MAKE_TRIANGLE(10, 14, 13);
        MAKE_TRIANGLE(11, 12, 15);
        MAKE_TRIANGLE(11, 15, 14);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_29:
        /* This case has four samples below the isosurface on the high
         * resolution face that form an L shape. There is one corresponding
         * sample below the isosurface on the low resolution face. The six
         * triangles generated form a strange shape that encloses the samples
         * below the isosurface.
         *
         * This corresponds to equivalence class #36 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 4, 11);
        MAKE_TRIANGLE(0, 11, 12);
        MAKE_TRIANGLE(0, 12, 15);
        MAKE_TRIANGLE(0, 15, 3);
        MAKE_TRIANGLE(3, 15, 14);
        MAKE_TRIANGLE(3, 14, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_30:
        /* This case has four samples below the isosurface on the high
         * resolution face that form a T shape. There are no samples below the
         * isosurface on the low resolution face. The five generated triangles
         * form an ugly polygon that caps the isosurface mesh.
         *
         * This corresponds to equivalence class #48 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(4, 5, 12);
        MAKE_TRIANGLE(4, 12, 10);
        MAKE_TRIANGLE(10, 12, 14);
        MAKE_TRIANGLE(10, 14, 13);
        MAKE_TRIANGLE(12, 15, 14);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_31:
        /* This case has four samples above the isosurface on the high
         * resolution face, with three of these samples in a line and the
         * fourth sample in a separate corner. There are three corresponding
         * samples above the isosurface on the low resolution face. Five
         * triangles are generated for this case.
         *
         * This corresponds to equivalence class #30 in \cite Lengyel:2010,
         * although the samples are inverted and thus the triangle winding
         * order is reversed.
         */
        MAKE_TRIANGLE(0, 5, 12);
        MAKE_TRIANGLE(0, 12, 15);
        MAKE_TRIANGLE(0, 15, 3);
        MAKE_TRIANGLE(3, 15, 14);
        MAKE_TRIANGLE(3, 14, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_32:
        /* This case has four samples above the isosurface on the high
         * resolution face, with three of these samples in a row along one of
         * the high resolution face edges, and the fourth sample on the edge
         * parallel to the first edge. This generates a trapezoidal polygon
         * made from three triangles, along with a separate fourth triangle to
         * cap the mesh.
         *
         * This corresponds to equivalence class #31 in \cite Lengyel:2010,
         * although the samples are inverted and thus the triangle winding
         * order is reversed.
         */
        MAKE_TRIANGLE(1, 15, 14);
        MAKE_TRIANGLE(1, 14, 3);
        MAKE_TRIANGLE(3, 14, 13);
        MAKE_TRIANGLE(4, 11, 5);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_33:
        /* This case has two samples below the isosurface on opposite corners
         * of the high resolution face. There are two corresponding samples on
         * the low resolution face that form an ambiguous face. A pair of quads
         * is generated by this cell configuration.
         *
         * This corresponds to equivalence class #8 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 5);
        MAKE_TRIANGLE(0, 1, 5);
        MAKE_TRIANGLE(2, 13, 8);
        MAKE_TRIANGLE(2, 3, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_34:
        /* This case has three samples below the isosurface in three corners of
         * the high resolution face. There are three corresponding samples
         * below the isosurface on the low resolution face. The six triangles
         * generated by this case make a concave shape that can be triangulated
         * in a couple of different ways.
         *
         * This corresponds to equivalence class #20 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 5);
        MAKE_TRIANGLE(1, 5, 4);
        MAKE_TRIANGLE(1, 4, 2);
        MAKE_TRIANGLE(2, 4, 10);
        MAKE_TRIANGLE(2, 10, 13);
        MAKE_TRIANGLE(2, 13, 8);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_35:
        /* This case has three samples below the isosurface on the high
         * resolution face. Two of these samples are in one corner of the high
         * resolution face, and the third sample is in the opposite corner.
         * There are two corresponding samples below the isosurface on the low
         * resolution face forming an ambiguous face. This generates a concave
         * trapezoid shape as well as a separate quad shape.
         *
         * This corresponds to equivalence class #18 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 11);
        MAKE_TRIANGLE(0, 1, 11);
        MAKE_TRIANGLE(0, 11, 4);
        MAKE_TRIANGLE(2, 13, 8);
        MAKE_TRIANGLE(2, 3, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_36:
        /* This case has four samples below the isosurface on the high
         * resolution face. Two of these samples are near one corner of the
         * high resolution face and the other two are near the opposite corner.
         * The two corresponding samples below the isosurface on the low
         * resolution face form an ambiguous face.  There is some symmetry to
         * this configuration, and the six triangles generated are grouped into
         * two polygons of three triangles each in opposite corners.
         *
         * This corresponds to equivalence class #41 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 11);
        MAKE_TRIANGLE(0, 1, 11);
        MAKE_TRIANGLE(0, 11, 4);
        MAKE_TRIANGLE(2, 6, 8);
        MAKE_TRIANGLE(2, 3, 6);
        MAKE_TRIANGLE(3, 10, 6);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_37:
        /* This case has three samples below the isosurface along the diagonal
         * of the high resolution face. Two corresponding samples below the
         * isosurface are on the low resolution face. This generates three
         * quads: one quad in the center of the high resolution face as a cap
         * on the mesh, and one quad on each of the two lateral edges below the
         * isosurface. This case has many ambiguous faces.
         *
         * This corresponds to equivalence class #24 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(6, 11, 7);
        MAKE_TRIANGLE(6, 7, 14);
        MAKE_TRIANGLE(1, 12, 5);
        MAKE_TRIANGLE(0, 1, 5);
        MAKE_TRIANGLE(2, 13, 8);
        MAKE_TRIANGLE(2, 3, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_38:
        /* This case has four samples below the isosurface on the high
         * resolution face, with three of them in corners and the fourth one in
         * the center. There are three corresponding samples below the
         * isosurface on the low resolution face. The triangles generated form
         * a quad as a cap on the mesh in the center of the high resolution
         * face, as well as a concave surface that circles around that quad.
         *
         * This corresponds to equivalence class #46 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(6, 11, 7);
        MAKE_TRIANGLE(6, 7, 14);
        MAKE_TRIANGLE(1, 12, 5);
        MAKE_TRIANGLE(1, 5, 4);
        MAKE_TRIANGLE(1, 4, 2);
        MAKE_TRIANGLE(2, 4, 10);
        MAKE_TRIANGLE(2, 10, 13);
        MAKE_TRIANGLE(2, 13, 8);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_39:
        /* This case has four samples below the isosurface on the high
         * resolution face. One of the high resolution cell faces is ambiguous.
         * Three of these samples form an L shape, and the fourth one is in a
         * separate corner. The two corresponding samples below the isosurface
         * on the low resolution face form an ambiguous face. The triangles
         * generated by this case form two polygons, one polygon with five
         * triangles, and the second polygon a quad.
         *
         * This corresponds to equivalence class #37 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 7);
        MAKE_TRIANGLE(1, 7, 14);
        MAKE_TRIANGLE(0, 1, 14);
        MAKE_TRIANGLE(0, 14, 6);
        MAKE_TRIANGLE(0, 6, 4);
        MAKE_TRIANGLE(2, 13, 8);
        MAKE_TRIANGLE(2, 3, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_40:
        /* This case has four samples above the isosurface on the high
         * resolution face. Since there is one ambiguous cell face on the high
         * resolution face, this equivalence class cannot be inverted.
         *
         * This corresponds to equivalence class #59 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 12, 7);
        MAKE_TRIANGLE(1, 7, 14);
        MAKE_TRIANGLE(1, 14, 2);
        MAKE_TRIANGLE(2, 14, 8);
        MAKE_TRIANGLE(8, 14, 13);
        MAKE_TRIANGLE(6, 13, 14);
        MAKE_TRIANGLE(6, 10, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_41:
        /* This case has five samples below the isosurface on the high
         * resolution face in an M shape. There are two corresponding samples
         * below the isosurface on the low resolution face that form an
         * ambiguous face. Since the low resolution face is ambiguous, this
         * equivalence class cannot be inverted. The triangles that this case
         * generates form a rather complicated tie-fighter shape.
         *
         * This corresponds to equivalence class #71 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 1, 12);
        MAKE_TRIANGLE(0, 12, 4);
        MAKE_TRIANGLE(4, 12, 7);
        MAKE_TRIANGLE(4, 7, 14);
        MAKE_TRIANGLE(4, 14, 10);
        MAKE_TRIANGLE(8, 10, 14);
        MAKE_TRIANGLE(3, 10, 8);
        MAKE_TRIANGLE(2, 3, 8);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_42:
        /* This case has three samples below the isosurface on the high
         * resolution face, with two of those samples in adjacent corners of
         * the high resolution face and the third sample on the opposite edge
         * of the high resolution face. There are two corresponding samples
         * below the isosurface on the low resolution face. The triangles
         * generated by this case form a scoop shape as well as a lone triangle
         * for a cap on the mesh. There is more than one way to triangulate the
         * scoop.
         *
         * This corresponds to equivalence class #21 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 4, 10);
        MAKE_TRIANGLE(0, 10, 13);
        MAKE_TRIANGLE(0, 13, 2);
        MAKE_TRIANGLE(2, 13, 8);
        MAKE_TRIANGLE(7, 12, 15);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_43:
        /* This case has three samples below the isosurface on the high
         * resolution face, with two of those samples on separate edges and one
         * of the samples in a corner. There is one corresponding sample below
         * the isosurface on the low resolution face. The triangles generated
         * by this case from two triangle caps on the mesh, as well as a quad
         * on the lateral edge that is below the isosurface. There is one
         * ambiguous cell face on the high resolution face.
         *
         * This corresponds to equivalence class #23 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(4, 5, 11);
        MAKE_TRIANGLE(7, 12, 15);
        MAKE_TRIANGLE(2, 13, 8);
        MAKE_TRIANGLE(2, 3, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_44:
        /* This case has four samples below the isosurface on the high
         * resolution face. One of these samples is on a lone edge of the high
         * resolution face and generates a single triangle to cap the mesh. The
         * other three samples generate an asymmetrical scoop shape. There are
         * two corresponding samples below the isosurface on the low resolution
         * face.
         *
         * This corresponds to equivalence class #38 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(7, 12, 15);
        MAKE_TRIANGLE(0, 5, 11);
        MAKE_TRIANGLE(0, 11, 10);
        MAKE_TRIANGLE(0, 10, 2);
        MAKE_TRIANGLE(2, 10, 13);
        MAKE_TRIANGLE(2, 13, 8);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_45:
        /* This case has four samples below the isosurface on the high
         * resolution face, with three of these samples in corners of the high
         * resolution face and one sample on an edge of the high resolution
         * face. There are three corresponding samples below the isosurface on
         * the low resolution face.
         *
         * This corresponds to equivalence class #43 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 7);
        MAKE_TRIANGLE(1, 7, 5);
        MAKE_TRIANGLE(1, 5, 4);
        MAKE_TRIANGLE(1, 4, 2);
        MAKE_TRIANGLE(2, 4, 10);
        MAKE_TRIANGLE(2, 10, 13);
        MAKE_TRIANGLE(2, 13, 8);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_46:
        /* This case has three samples below the isosurface in one corner of
         * the high resolution face, and a fourth sample below the isosurface
         * in the opposite corner of the high resolution face. There are two
         * corresponding samples below the isosurface on the low resolution
         * face that form an ambiguous face. The triangles generated by this
         * case form a trapezoid made from four triangles along one of the
         * lateral edges, and a quad along the opposite lateral edge.
         *
         * This corresponds to equivalence class #40 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 7);
        MAKE_TRIANGLE(1, 7, 11);
        MAKE_TRIANGLE(0, 1, 11);
        MAKE_TRIANGLE(0, 11, 4);
        MAKE_TRIANGLE(2, 13, 8);
        MAKE_TRIANGLE(2, 3, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_47:
        /* This case has four samples below the isosurface on the high
         * resolution face, with two of those samples on separate edges
         * generating two separate triangles that cap the mesh, and the other
         * two samples generating three triangles along a lateral edge. There
         * is a single sample below the isosurface on the low resolution face.
         *
         * This corresponds to equivalence class #39 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(4, 5, 11);
        MAKE_TRIANGLE(7, 12, 15);
        MAKE_TRIANGLE(3, 10, 6);
        MAKE_TRIANGLE(2, 3, 6);
        MAKE_TRIANGLE(2, 6, 8);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_48:
        /* This case has six samples below the isosurface on the high
         * resolution face in the shape of the glider from Conway's Game of
         * Life. Two corresponding samples below the isosurface are on the
         * low resolution face. One of the cell faces on the high resolution
         * face is ambiguous. One triangle is generated as a cap for the mesh,
         * and four additional triangles are generated in a swooping shape.
         *
         * This corresponds to equivalence class #60 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(7, 12, 15);
        MAKE_TRIANGLE(0, 5, 11);
        MAKE_TRIANGLE(0, 11, 6);
        MAKE_TRIANGLE(0, 6, 2);
        MAKE_TRIANGLE(2, 6, 8);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_49:
        /* This case has four samples below the isosurface on the high
         * resolution face. Two corresponding samples below the isosurface are
         * on the low resolution face, which make an ambiguous face.
         *
         * This corresponds to equivalence class #44 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 7);
        MAKE_TRIANGLE(0, 1, 7);
        MAKE_TRIANGLE(0, 7, 5);
        MAKE_TRIANGLE(2, 6, 8);
        MAKE_TRIANGLE(2, 3, 6);
        MAKE_TRIANGLE(3, 10, 6);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_50:
        /* This case has five samples below the isosurface on the high
         * resolution face. Two corresponding samples below the isosurface are
         * on the low resolution face, which make an ambiguous face. There is
         * also an ambiguous cell face on the high resolution face.
         *
         * This corresponds to equivalence class #61 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 7);
        MAKE_TRIANGLE(1, 7, 11);
        MAKE_TRIANGLE(0, 1, 11);
        MAKE_TRIANGLE(0, 11, 4);
        MAKE_TRIANGLE(2, 6, 8);
        MAKE_TRIANGLE(2, 3, 6);
        MAKE_TRIANGLE(3, 10, 6);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_51:
        /* This case has four samples below the isosurface on the high
         * resolution face. Two of these samples are in adjacent corners of the
         * high resolution face, and the triangles they generate form a scoop
         * shape. The other two samples generate a triangle cap in a pentagon
         * or "doll house" shape.
         *
         * This corresponds to equivalence class #47 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 4, 10);
        MAKE_TRIANGLE(0, 10, 13);
        MAKE_TRIANGLE(0, 13, 2);
        MAKE_TRIANGLE(2, 13, 8);
        MAKE_TRIANGLE(6, 11, 14);
        MAKE_TRIANGLE(11, 12, 14);
        MAKE_TRIANGLE(12, 15, 14);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_52:
        /* This case has four samples below the isosurface on the high
         * resolution face in a Y shape. There is one corresponding sample
         * below the isosurface on the low resolution face. One of the cell
         * faces on the high resolution face is ambiguous. Four of the
         * triangles generated by this case form a hexagon cap on the mesh.
         * The other two triangles generated by this case form a quad along the
         * lateral edge below the isosurface.
         *
         * This corresponds to equivalence class #49 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(12, 15, 14);
        MAKE_TRIANGLE(5, 12, 14);
        MAKE_TRIANGLE(5, 14, 6);
        MAKE_TRIANGLE(4, 5, 6);
        MAKE_TRIANGLE(2, 13, 8);
        MAKE_TRIANGLE(2, 3, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_53:
        /* This case has five samples below the isosurface on the high
         * resolution face in a Y shape with an additional sample. There is one
         * ambiguous cell face on the high resolution face. The eight triangles
         * generated by this case form a single surface that is fairly
         * complicated
         *
         * This corresponds to equivalence class #62 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 5, 12);
        MAKE_TRIANGLE(0, 12, 15);
        MAKE_TRIANGLE(0, 15, 2);
        MAKE_TRIANGLE(2, 15, 14);
        MAKE_TRIANGLE(2, 14, 8);
        MAKE_TRIANGLE(6, 8, 14);
        MAKE_TRIANGLE(6, 13, 8);
        MAKE_TRIANGLE(6, 10, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_54:
        /* This case has five samples below the isosurface on the high
         * resolution face, and three corresponding samples below the
         * isosurface on the low resolution face. Two cell faces on the high
         * resolution face are ambiguous. The triangles generated by this case
         * form a cap on the mesh in a C shape along with three around the
         * lateral edge above the isosurface.
         *
         * This corresponds to equivalence class #63 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(4, 11, 5);
        MAKE_TRIANGLE(4, 10, 11);
        MAKE_TRIANGLE(6, 11, 10);
        MAKE_TRIANGLE(6, 10, 13);
        MAKE_TRIANGLE(6, 13, 14);
        MAKE_TRIANGLE(14, 13, 8);
        MAKE_TRIANGLE(2, 14, 8);
        MAKE_TRIANGLE(1, 14, 2);
        MAKE_TRIANGLE(1, 15, 14);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_55:
        /* This case has five samples below the isosurface on the high
         * resolution face. Four of these samples are in a square in one
         * corner, and the fifth sample is in the opposite corner. There are
         * two corresponding samples below the isosurface on the low resolution
         * face, which makes an ambiguous face. There is also one ambigous cell
         * face on the high resolution face. The triangles that this case
         * generates form two surfaces along the lateral edges below the
         * isosurface.
         *
         * This corresponds to equivalence class #64 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 14);
        MAKE_TRIANGLE(0, 1, 14);
        MAKE_TRIANGLE(0, 14, 6);
        MAKE_TRIANGLE(0, 6, 4);
        MAKE_TRIANGLE(2, 13, 8);
        MAKE_TRIANGLE(2, 3, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_56:
        /* This case has six samples below the isosurface on the high
         * resolution face. There is one ambiguous cell face on the high
         * resolution face. There are three samples below the isosurface on the
         * low resolution face.
         *
         * This corresponds to equivalence class #53 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(6, 10, 13);
        MAKE_TRIANGLE(6, 13, 14);
        MAKE_TRIANGLE(8, 14, 13);
        MAKE_TRIANGLE(2, 14, 8);
        MAKE_TRIANGLE(1, 14, 2);
        MAKE_TRIANGLE(1, 15, 14);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_57:
        /* This case has five samples below the isosurface on the high
         * resolution face in an "S" shape. Two corresponding samples below the
         * isosurface on the low resolution face form an ambiguous face. The
         * eight triangles generated by this case form a tie-fighter shape.
         *
         * This corresponds to equivalence class #72 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 1, 5);
        MAKE_TRIANGLE(1, 15, 5);
        MAKE_TRIANGLE(5, 15, 11);
        MAKE_TRIANGLE(11, 15, 14);
        MAKE_TRIANGLE(10, 11, 14);
        MAKE_TRIANGLE(8, 10, 14);
        MAKE_TRIANGLE(3, 10, 8);
        MAKE_TRIANGLE(2, 3, 8);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_58:
        /* This case has six samples below the isosurface on the high
         * resolution face. There are two corresponding samples below the
         * isosurface on the low resolution face that form an ambiguous face.
         * The triangles generated by this case make a tie-fighter shape.
         *
         * This corresponds to equivalence class #70 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 1, 15);
        MAKE_TRIANGLE(0, 15, 4);
        MAKE_TRIANGLE(4, 15, 14);
        MAKE_TRIANGLE(4, 14, 10);
        MAKE_TRIANGLE(8, 10, 14);
        MAKE_TRIANGLE(2, 10, 8);
        MAKE_TRIANGLE(2, 3, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_59:
        /* This case has four samples below the isosurface on the high
         * resolution face, each on a different edge. This generates four
         * triangle caps for the mesh. The low resolution face has no samples
         * below the isosurface.
         *
         * This corresponds to equivalence class #50 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(4, 5, 11);
        MAKE_TRIANGLE(6, 13, 10);
        MAKE_TRIANGLE(7, 12, 15);
        MAKE_TRIANGLE(8, 14, 9);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_60:
        /* This case has five samples below the isosurface on the high
         * resolution face, with three of the face cells on the high resolution
         * face being ambiguous. Two of these samples are on high resolution
         * face edges and generate triangles to cap the mesh. The remaining
         * three samples generate four triangles in a surface that is nearly a
         * trapezoid.
         *
         * This corresponds to equivalence class #65 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(7, 12, 15);
        MAKE_TRIANGLE(8, 14, 9);
        MAKE_TRIANGLE(0, 5, 11);
        MAKE_TRIANGLE(0, 11, 6);
        MAKE_TRIANGLE(0, 6, 3);
        MAKE_TRIANGLE(3, 6, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_61:
        /* This case has five samples below the isosurface on the high
         * resolution face, with two ambiguous cell faces on the high
         * resolution face. There are two samples below the isosurface on the
         * low resolution face. The triangles generated form a deep scoop shape
         * as well as a lone triangle as a cap on the mesh.
         *
         * This corresponds to equivalence class #66 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 7);
        MAKE_TRIANGLE(1, 7, 5);
        MAKE_TRIANGLE(1, 5, 4);
        MAKE_TRIANGLE(1, 4, 3);
        MAKE_TRIANGLE(3, 4, 6);
        MAKE_TRIANGLE(3, 6, 13);
        MAKE_TRIANGLE(8, 14, 9);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_62:
        /* This case has six samples below the isosurface on the high
         * resolution face, with two ambiguous cell faces on the high
         * resolution face. There are two samples below the isosurface on the
         * low resolution face. The triangles generated by this case form an
         * angular scoop shape, as well as a lone triangle that caps the mesh.
         *
         * This corresponds to equivalence class #54 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 7);
        MAKE_TRIANGLE(1, 7, 11);
        MAKE_TRIANGLE(1, 11, 3);
        MAKE_TRIANGLE(3, 11, 6);
        MAKE_TRIANGLE(3, 6, 13);
        MAKE_TRIANGLE(8, 14, 9);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_63:
        /* This case has five samples below the isosurface on the high
         * resolution face in a plus sign arrangement. There are no samples on
         * the low resolution face below the isosurface. The six triangles
         * generated by this case form an octogon shape.
         *
         * This corresponds to equivalence class #45 in \cite Lengyel:2010.
         * Note that the samples are inverted when compared to Lengyel's
         * equivalence class, so the winding order is reversed.
         */
        MAKE_TRIANGLE(9, 12, 15);
        MAKE_TRIANGLE(5, 12, 9);
        MAKE_TRIANGLE(4, 5, 9);
        MAKE_TRIANGLE(4, 9, 8);
        MAKE_TRIANGLE(4, 8, 10);
        MAKE_TRIANGLE(8, 13, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_64:
        /* This case has five samples below the isosurface on the high
         * resolution face. One cell face on the high resolution face is
         * ambiguous. There are three samples below the isosurface on the low
         * resolution face.
         *
         * This corresponds to equivalence class #67 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 7);
        MAKE_TRIANGLE(1, 7, 2);
        MAKE_TRIANGLE(2, 7, 14);
        MAKE_TRIANGLE(2, 14, 9);
        MAKE_TRIANGLE(10, 13, 14);
        MAKE_TRIANGLE(7, 10, 14);
        MAKE_TRIANGLE(4, 10, 7);
        MAKE_TRIANGLE(4, 7, 5);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_65:
        /* This case has six samples below the isosurface on the high
         * resolution face. One cell face on the high resolution face is
         * ambiguous. There are three samples below the isosurface on the low
         * resolution face.
         *
         * This corresponds to equivalence class #55 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 7);
        MAKE_TRIANGLE(1, 7, 2);
        MAKE_TRIANGLE(2, 7, 14);
        MAKE_TRIANGLE(2, 14, 9);
        MAKE_TRIANGLE(7, 11, 14);
        MAKE_TRIANGLE(11, 13, 14);
        MAKE_TRIANGLE(10, 13, 11);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_66:
        /* This case has six samples below the isosurface on the high
         * resolution face, with three of these samples concentrated in one
         * corner and the other three concentrated in the opposite corner.
         * There are two ambiguous cell faces on the high resolution face.
         * There are two samples below the isosurface on the low resolution
         * face making an ambiguous face. The triangles generated form two
         * opposing trapezoid shapes each composed of four triangles.
         *
         * This corresponds to equivalence class #57 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 7);
        MAKE_TRIANGLE(1, 7, 11);
        MAKE_TRIANGLE(0, 1, 11);
        MAKE_TRIANGLE(0, 11, 4);
        MAKE_TRIANGLE(2, 14, 9);
        MAKE_TRIANGLE(2, 6, 14);
        MAKE_TRIANGLE(2, 3, 6);
        MAKE_TRIANGLE(3, 10, 6);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_67:
        /* This case has seven samples below the isosurface on the high
         * resolution face, with one ambigous cell face on the high resolution
         * face. There are three samples below the isosurface on the low
         * resolution face.
         *
         * This corresponds to equivalence class #51 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 7);
        MAKE_TRIANGLE(1, 7, 2);
        MAKE_TRIANGLE(2, 7, 14);
        MAKE_TRIANGLE(2, 14, 9);
        MAKE_TRIANGLE(7, 11, 14);
        MAKE_TRIANGLE(6, 14, 11);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_68:
        /* This case has six samples below the isosurface on the high
         * resolution face. There is one ambiguous cell face on the high
         * resolution face. The low resolution face has three samples below the
         * isosurface. The triangles generated by this case form a quad along
         * the lateral edge that is above the isosurface, as well as a separate
         * triangle strip that caps the mesh.
         *
         * This corresponds to equivalence class #56 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(1, 15, 9);
        MAKE_TRIANGLE(1, 9, 2);
        MAKE_TRIANGLE(4, 11, 5);
        MAKE_TRIANGLE(4, 10, 11);
        MAKE_TRIANGLE(6, 11, 10);
        MAKE_TRIANGLE(6, 10, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_69:
        /* This case has seven samples below the isosurface on the high
         * resolution face. There are two corresponding samples below the
         * isosurface on the low resolution face that form an ambiguous face.
         * The six triangles generated by this case form a tie-fighter shape.
         *
         * This corresponds to equivalence class #69 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(0, 1, 4);
        MAKE_TRIANGLE(1, 15, 4);
        MAKE_TRIANGLE(4, 15, 9);
        MAKE_TRIANGLE(4, 9, 10);
        MAKE_TRIANGLE(2, 10, 9);
        MAKE_TRIANGLE(2, 3, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_70:
        /* This case has five samples below the isosurface on the high
         * resolution face, with all four cell faces on the high resolution
         * face being ambiguous. All four samples on the low resolution face
         * are below the isosurface. The twelve triangles generated by this
         * case form an octogonal triangle strip mesh cap with a hole in the
         * center.
         *
         * This corresponds to equivalence class #68 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(4, 10, 11);
        MAKE_TRIANGLE(4, 11, 5);
        MAKE_TRIANGLE(5, 11, 12);
        MAKE_TRIANGLE(7, 12, 11);
        MAKE_TRIANGLE(7, 15, 12);
        MAKE_TRIANGLE(7, 14, 15);
        MAKE_TRIANGLE(9, 15, 14);
        MAKE_TRIANGLE(8, 9, 14);
        MAKE_TRIANGLE(8, 14, 13);
        MAKE_TRIANGLE(6, 13, 14);
        MAKE_TRIANGLE(6, 10, 13);
        MAKE_TRIANGLE(6, 11, 10);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_71:
        /* This case has six samples below the isosurface on the high
         * resolution face, with two of the cell faces on the high resolution
         * face being ambiguous. All four samples on the low resolution face
         * are below the isosurface. The triangles generated by this case form
         * a cap on the mesh in a pacman shape.
         *
         * This corresponds to equivalence class #58 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(7, 15, 12);
        MAKE_TRIANGLE(7, 14, 15);
        MAKE_TRIANGLE(9, 15, 14);
        MAKE_TRIANGLE(8, 9, 14);
        MAKE_TRIANGLE(8, 14, 13);
        MAKE_TRIANGLE(6, 13, 14);
        MAKE_TRIANGLE(6, 10, 13);
        break;
      case MC_TRANSVOXEL_CANONICAL_TRANSITION_CELL_72:
        /* This case has seven samples below the isosurface on the high
         * resolution face, with one ambiguous cell face on the high resolution
         * face. All four samples on the low resolution face are below the
         * isosurface. The four triangles generated by this case form a
         * trapezoidal cap on the mesh.
         *
         * This corresponds to equivalence class #52 in \cite Lengyel:2010.
         */
        MAKE_TRIANGLE(7, 15, 12);
        MAKE_TRIANGLE(7, 14, 15);
        MAKE_TRIANGLE(9, 15, 14);
        MAKE_TRIANGLE(8, 9, 14);
        break;
      default:
        assert(0);
    }

    /* TODO: Apply the rotation/reflection/inversion sequence in reverse to
     * arrive at the actual edge intersections for our triangles. */
    for (int i = 0; i < MC_TRANSVOXEL_MAX_TRANSITION_CELL_TRIANGLES; ++i) {
      mcTransvoxel_Triangle *triangle = &list->triangles[i];
      if (triangle->edgeIntersections[0] == -1)
        break;  /* No more triangles to conisder */
      /* Iterate over each triangle edge intersection */
      for (int j = 0; j < 3; ++j) {
        int *edge = &triangle->edgeIntersections[j];
        assert(*edge != -1);
        /* Rotate the triangle edge intersection about the z-axis */
        for (int k = 0; k < get_byte(sequence, 0); ++k) {
          *edge = mcTransvoxel_rotateTransitionCellEdgeReverse(*edge);
        }
        /* Mirror the triangle edge intersection */
        if (get_byte(sequence, 1)) {
          *edge = mcTransvoxel_reflectTransitionCellEdge(*edge);
        }
      }
      /* Transition cell inversion affects triangle winding order */
      if (get_byte(sequence, 2)) {
        /* Reverse triangle winding order to get correct front/back faces */
        int temp = triangle->edgeIntersections[0];
        triangle->edgeIntersections[0] = triangle->edgeIntersections[2];
        triangle->edgeIntersections[2] = temp;
      }
    }
  }
}

void mcTransvoxel_printTransitionCellTriangulationTable(
    mcTransvoxel_TransitionCellTriangleList *table,
    FILE *fp)
{
  fprintf(fp,
      "const mcTransvoxel_TransitionCellTriangleList\n"
      "mcTransvoxel_transitionCellTriangulationTable[] = {\n");
  /* Loop through all of the transition cell configurations */
  for (int cell = 0; cell < MC_TRANSVOXEL_NUM_TRANSITION_CELLS; ++cell) {
    mcTransvoxel_TransitionCellTriangleList *list = &table[cell];
    fprintf(fp,
        "  {  /* Transition Cell 0x%03x */ \n"
        "    .triangles = {\n",
        cell);
    /* Loop through the list of triangles for this cell */
    for (int i = 0; i < MC_TRANSVOXEL_MAX_TRANSITION_CELL_TRIANGLES; ++i) {
      mcTransvoxel_Triangle *triangle = &list->triangles[i];
      fprintf(fp,
          "      { .edgeIntersections = { ");
      /* Loop and output the edge intersections for this triangle */
      for (int j = 0; j < 3; ++j) {
        int edge = triangle->edgeIntersections[j];
        fprintf(fp, "%d, ", edge);
      }
      fprintf(fp,
          "}, },\n");
    }
    fprintf(fp,
        "    },\n"
        "  },\n");
  }
  fprintf(fp, "};\n");
}

void print_usage() {
  fprintf(stderr,
      "Usage:\n"
      "transvoxel_generate_triangulation_tables [filename]\n\n"
      "Where [filename] is one of the following:\n"
      "    transvoxel_triangulation_tables.c\n"
      );
}

int main(int argc, char **argv) {
  enum {
    TRANSVOXEL_TRIANGULATION_TABLES_C,
  } output;

  mcTransvoxel_TransitionCellTriangleList *transitionCellTriangulationTable;

  /* Parse the arguments to determine which file we are generating */
  if (argc != 2) {
    print_usage();
    return EXIT_FAILURE;
  }
  if (strcmp(argv[1], "transvoxel_triangulation_tables.c") == 0) {
    output = TRANSVOXEL_TRIANGULATION_TABLES_C;
  } else {
    print_usage();
    return EXIT_FAILURE;
  }

  /* Allocate memory for each of our tables */
  transitionCellTriangulationTable = 
    (mcTransvoxel_TransitionCellTriangleList *)malloc(
        sizeof(mcTransvoxel_TransitionCellTriangleList)
        * MC_TRANSVOXEL_NUM_TRANSITION_CELLS);

  /* Compute the tables */
  mcTransvoxel_computeTransitionCellTriangulationTable(
      transitionCellTriangulationTable);

  /* Print the tables */
  /* NOTE: stdout is used because Emscripten's filesystem model makes using
   * fopen() difficult */
  switch (output) {
    case TRANSVOXEL_TRIANGULATION_TABLES_C:
      mcTransvoxel_printTransitionCellTriangulationTable(
          transitionCellTriangulationTable, stdout);
      break;
  }
}
