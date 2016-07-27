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

#ifndef MC_ALGORITHMS_TRANSVOXEL_CANONICAL_H_
#define MC_ALGORITHMS_TRANSVOXEL_CANONICAL_H_

extern const int mcTransvoxel_canonicalRegularCells[];
extern const int mcTransvoxel_canonicalTransitionCells[];

int mcTransvoxel_canonicalRegularCell(int cell);
int mcTransvoxel_canonicalRegularCellSequence(int cell);

/**
 * This routine finds the canonical transition cell configuration for the given
 * transition cell configuration. The canonical configuration is computed using
 * the 73 equivalence classes as given by Lengyel in \cite Lengyel:2010.
 *
 * \param cell The transition cell configuration of which to look for the
 * canonical transition cell configuration.
 * \return The canonical transition cell configuration for the given transition
 * cell configuration.
 *
 * \sa mcTransvoxel_canonicalTransitionCellSequence()
 */
int mcTransvoxel_canonicalTransitionCell(int cell);

/**
 * This routine finds the rotation/reflection/inversion sequence of transforms
 * needed to transform the given transition cell configuration into its
 * canonical configuration. This sequence is needed in order to transform the
 * canonical set of triangles to the actual triangles needed for the given
 * transition cell configuration.
 *
 * The transform sequence is encoded in the bytes of the integer returned in
 * the following way. The low order byte gives the number of rotations used to
 * transform the transition cell. Rotations are performed by the
 * mcTransvoxel_rotateTransitionCell() routine and can be reversed with the
 * mcTransvoxel_rotateTransitionCellReverse() rotuine. The next byte is set to
 * one if the transition cell was mirrored, and zero otherwise. The third byte
 * is set to one if the transition cell was inverted, and zero otherwise.
 *
 * The transformations are performed in the order of inversion, reflection,
 * rotation, so to reverse the transformation the reverse of those  operations
 * need to be performed in the opposite order, i.e. rotation followed by
 * reflection followed by inversion.
 *
 * \sa mcTransvoxel_canonicalTransitionCell()
 */
int mcTransvoxel_canonicalTransitionCellSequence(int cell);

#endif
