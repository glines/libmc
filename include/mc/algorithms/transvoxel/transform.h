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

#ifndef MC_ALGORITHMS_TRANSVOXEL_TRANSFORM_H_
#define MC_ALGORITHMS_TRANSVOXEL_TRANSFORM_H_

/**
 * This routine inverts the given transition cell configuration with a simple
 * bitwise operation. Inversion is needed for some of the transition cell
 * equivalence classes.
 *
 * \param cell The transition cell configuration to invert.
 * \return The transition cell configuration with each sample inverted.
 */
int mcTransvoxel_invertTransitionCell(int cell);

/**
 * This routine computes the reflection of the given transition cell. The
 * reflection is needed for applying the transition cell equivalence classes to
 * cell configurations that are not symmetrical. The axis over which the
 * transition cell is reflected is not important.
 *
 * \param cell the transition cell configuration to reflect.
 * \return The reflected transition cell configuration.
 */
int mcTransvoxel_reflectTransitionCell(int cell);

/**
 * This routine performs the transition cell rotation needed in order to
 * manipulate the transition cells in and out of their equivalence classes.
 *
 * \param cell The transition cell configuration to rotate.
 * \return The rotated transition cell configuration.
 *
 * \sa mcTransvoxel_invertTransitionCell()
 * \sa mcTransvoxel_rotateTransitionCell()
 */
int mcTransvoxel_rotateTransitionCell(int cell);

/**
 * This routine rotates the given transition cell edge.
 *
 * \param edge Index of the transition cell edge to rotate.
 * \return The index of the rotated transition cell edge.
 *
 * \sa mcTransvoxel_rotateTranisitonCellEdgeReverse()
 */
int mcTransvoxel_rotateTransitionCellEdge(int edge);

/**
 * This routine rotates the given transition cell edge in the reverse direction
 * as mcTransvoxel_rotateTransitionCellEdge(). This routine is needed in order
 * to rotate the edge intersections for canonical transition cell
 * triangulations into their final edge intersection indices.
 *
 * Note that due to the square symmetry of the transition cells, this is
 * equivalent to calling mcTransvoxel_rotateTransitionCellEdge() three times.
 *
 * \param edge Index of the transition cell edge to rotate in reverse.
 * \return Index of the transition cell edge rotated in reverse by 90 degrees.
 *
 * \sa mcTransvoxel_rotateTransitionCellEdge()
 */
int mcTransvoxel_rotateTransitionCellEdgeReverse(int edge);

/**
 * This routine reflects the given transition cell edge index about the same
 * axis as mcTransvoxel_reflectTransitionCell(). This routine is needed to
 * transform transition cell edge intersections for canonical triangulations
 * into the actual transition cell edge intersections.
 *
 * \param edge Index of the transition cell edge.
 * \return Index of the reflected transition cell edge.
 */
int mcTransvoxel_reflectTransitionCellEdge(int edge);

/**
 * This routine finds the relative position of the gives sample and returns the
 * result as a pair of integers. The relative position is measured from sample
 * 0 at the relative position (0, 0), with sample 8 at the relative position
 * (2, 2). See the figure of sample positions, as given by Legyel in Figure
 * 4.16 of \cite Legyel:2010.
 *
 * \param sampleIndex Index of the transition cell sample.
 * \param pos The relative position of this transition cell sample is returned
 * as two integers in this argument.
 */
void mcTransvoxel_transitionCellSampleRelativePosition(
    int sampleIndex, int *pos);

#endif
