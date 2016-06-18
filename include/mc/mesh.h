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

#ifndef MC_MESH_H_
#define MC_MESH_H_

/**
 * \addtogroup libmc
 * @{
 */

/**
 * \defgroup mcMesh mcMesh
 */

/**
 * \addtogroup mcMesh
 * @{
 */

/** \file mc/mesh.h
 *
 * This file contains definitions for the mcMesh structure and the structures
 * that it contains.
 */

#include "vector.h"

typedef struct mcVertex {
  mcVec3 pos;
  mcVec3 norm;
} mcVertex;

typedef struct mcFace {
  unsigned int *indices;
  unsigned int numIndices;
} mcFace;

/**
 * Initializes a mcFace structure.
 *
 * Since libmc supports any number of edges for a face, it becomes necessary to
 * dynamically allocate the appropriate number of face indices for any given
 * face structure. Because dynamically allocating memory for many such small
 * structures would incur a high overhead cost, the library maintains an
 * internal pool of memory for common face sizes such as triangles and quads.
 */
void mcFace_init(
    mcFace *self,
    unsigned int numIndices);

/**
 * Copies a mcFace structure into another mcFace structure.
 *
 * \param self The mcFace structure being initalized with the copied data.
 * \param other The mcFace structure whose data is to be copied.
 *
 * This routine is analogous to a copy constructor in C++; the face structure
 * pointed to by \p self should be uninitialized before this routine is called.
 * After this routine is called, \p self should be treated as an initialized
 * mcFace structure, and should eventually be destroyed with a call to
 * mcFace_destroy().
 */
void mcFace_copy(
    mcFace *self,
    const mcFace *other);

void mcFace_move(
    mcFace *self,
    mcFace *other);

void mcFace_destroy(
    mcFace *self);

typedef struct mcMesh {
  mcVertex *vertices;
  mcFace *faces;
  unsigned int numVertices, numFaces, numIndices;
  unsigned int sizeVertices, sizeFaces;
  int isTriangleMesh;
} mcMesh;

void mcMesh_init(
    mcMesh *self);

void mcMesh_destroy(
    mcMesh *self);

void mcMesh_growVertices(
    mcMesh *self);

void mcMesh_growFaces(
    mcMesh *self);

unsigned int mcMesh_addVertex(
    mcMesh *self,
    const mcVertex *vertex);

void mcMesh_addFace(
    mcMesh *self,
    const mcFace *face);

/** @} */

/** @} */

#endif
