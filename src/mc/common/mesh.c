#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include <mc/mesh.h>

void mcMesh_init(
    mcMesh *self)
{
  /* Initialize an empty mesh. Subsequent calls to mcMesh_grow() will allocate
   * memory for the mesh. */
  self->vertices = NULL;
  self->numVertices = 0;
  self->faces = NULL;
  self->numFaces = 0;
}

void mcMesh_destroy(
    mcMesh *self)
{
  free(self->faces);
  free(self->vertices);
}

void mcMesh_grow(
    mcMesh *self)
{
  /* TODO */
}

void mcMesh_addVertex(
    mcMesh *self,
    const mcVertex *vertex)
{
  /* TODO */
  assert(0);
}

void mcMesh_addFace(
    mcMesh *self,
    const mcFace *face)
{
  /* TODO */
}
