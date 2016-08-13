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

#include <stdlib.h>
#include <string.h>

#include <mc/contour.h>

#define MC_CONTOUR_INIT_SIZE_VERTICES 16
#define MC_CONTOUR_INIT_SIZE_LINES 8

void mcContour_init(mcContour *self) {
  self->numVertices = 0;
  self->numLines = 0;
  self->sizeVertices = MC_CONTOUR_INIT_SIZE_VERTICES;
  self->vertices = (mcVertex*)malloc(sizeof(mcVertex) * self->sizeVertices);
  self->sizeLines = MC_CONTOUR_INIT_SIZE_LINES;
  self->lines = (mcLine*)malloc(sizeof(mcLine) * self->sizeLines);
}

void mcContour_growVertices(mcContour *self) {
  /* Double the size of our vertices buffer */
  mcVertex *newVertices =
    (mcVertex*)malloc(sizeof(mcVertex) * self->sizeVertices * 2);
  memcpy(newVertices, self->vertices, sizeof(mcVertex) * self->sizeVertices);
  free(self->vertices);
  self->vertices = newVertices;
  self->sizeVertices *= 2;
}

void mcContour_growLines(mcContour *self) {
  /* Double the size of our lines buffer */
  mcLine *newLines =
    (mcLine*)malloc(sizeof(mcLine) * self->sizeLines * 2);
  memcpy(newLines, self->lines, sizeof(mcLine) * self->sizeLines);
  free(self->lines);
  self->lines = newLines;
  self->sizeLines *= 2;
}

int mcContour_addVertex(mcContour *self, const mcVertex *vertex) {
  /* Make sure we have enough memory allocated for this vertex */
  if (self->numVertices >= self->sizeVertices) {
    mcContour_growVertices(self);
  }
  /* Add the vertex and increment the vertex index */
  self->vertices[self->numVertices++] = *vertex;
  return self->numVertices - 1;
}

void mcContour_addLine(mcContour *self, const mcLine *line) {
  /* Make sure that we have enough memory allocated for this line */
  if (self->numLines >= self->sizeLines) {
    mcContour_growLines(self);
  }
  /* Add the line and increment the lines index */
  self->lines[self->numLines++] = *line;
}
