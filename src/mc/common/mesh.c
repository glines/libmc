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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <mc/mesh.h>

void mcFace_init(
    mcFace *self,
    unsigned int numIndices)
{
  /* Allocate memory for the indices */
  /* TODO: This malloc is a prime candidate for a pool allocator */
  self->indices =
    (unsigned int *)malloc(sizeof(unsigned int) * numIndices);
  self->numIndices = numIndices;
}

void mcFace_copy(
    mcFace *self,
    const mcFace *other)
{
  /* Allocate memory for the indices and copy them */
  /* TODO: This malloc is a prime candidate for a pool allocator */
  self->indices =
    (unsigned int *)malloc(sizeof(unsigned int) * other->numIndices);
  memcpy(self->indices, other->indices, sizeof(unsigned int) * other->numIndices);
  self->numIndices = other->numIndices;
}

void mcFace_destroy(
    mcFace *self)
{
  free(self->indices);
}


void mcMesh_init(
    mcMesh *self)
{
  const unsigned int INIT_SIZE_VERTICES = 1024;
  const unsigned int INIT_SIZE_FACES = 1024;

  /* Initialize an empty mesh. Subsequent calls to mcMesh_grow() will allocate
   * memory for the mesh. */
  self->vertices = malloc(sizeof(mcVertex) * INIT_SIZE_VERTICES);
  self->sizeVertices = INIT_SIZE_VERTICES;
  self->numVertices = 0;
  self->faces = malloc(sizeof(mcFace) * INIT_SIZE_FACES);
  self->sizeFaces = INIT_SIZE_FACES;
  self->numFaces = 0;
}

void mcMesh_destroy(
    mcMesh *self)
{
  for (unsigned int i = 0; i < self->numFaces; ++i) {
    mcFace_destroy(&self->faces[i]);
  }
  for (unsigned int i = 0; i < self->numVertices; ++i) {
    /* mcVertex_destroy(self->vertices[i]); */ /* TODO? */
  }

  free(self->faces);
  free(self->vertices);
}

void mcMesh_growVertices(
    mcMesh *self)
{
  /* Double the size of our vertices buffer */
  mcVertex *newVertices =
    (mcVertex*)malloc(sizeof(mcVertex) * self->sizeVertices * 2);
  memcpy(newVertices, self->vertices, sizeof(mcVertex) * self->sizeVertices);
  free(self->vertices);
  self->vertices = newVertices;
  self->sizeVertices *= 2;
}

void mcMesh_growFaces(
    mcMesh *self)
{
  /* Double the size of our faces buffer */
  mcFace *newFaces =
    (mcFace*)malloc(sizeof(mcFace) * self->sizeFaces * 2);
  memcpy(newFaces, self->faces, sizeof(mcFace) * self->sizeFaces);
  free(self->faces);
  self->faces = newFaces;
  self->sizeFaces *= 2;
}

unsigned int mcMesh_addVertex(
    mcMesh *self,
    const mcVertex *vertex)
{
  /* Make sure we have enough memory allocated for this vertex */
  if (self->numVertices >= self->sizeVertices) {
    mcMesh_growVertices(self);
  }
  /* Add the vertex and increment the vertex index */
  self->vertices[self->numVertices++] = *vertex;
  return self->numVertices - 1;
}

void mcMesh_addFace(
    mcMesh *self,
    const mcFace *face)
{
  /* Make sure we have enough memory allocated for this face */
  if (self->numFaces >= self->sizeFaces) {
    mcMesh_growFaces(self);
  }
  /* Add the face to the mesh */
  /* TODO: This mcFace_copy can be replaced with mcFace_move */
  mcFace_copy(&self->faces[self->numFaces++], face);
}
