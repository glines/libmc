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
#include <stdlib.h>
#include <string.h>

#include <mc/algorithms/marchingSquares.h>
#include <mc/contourBuilder.h>

struct mcContourBuilderInternal {
  mcContour *contours;
  int contoursSize;
  int numContours;
};

void mcContourBuilder_init(mcContourBuilder *self) {
  static const int INIT_NUM_CONTOURS = 4;

  /* Allocate memory for the internal data structures */
  self->internal =
    (mcContourBuilderInternal*)malloc(sizeof(mcContourBuilderInternal));
  self->internal->contours =
    (mcContour*)malloc(sizeof(mcContour) * INIT_NUM_CONTOURS);
  self->internal->contoursSize = INIT_NUM_CONTOURS;
  self->internal->numContours = 0;
}

void mcContourBuilder_destroy(mcContourBuilder *self) {
  /* Destroy all initialized contours */
  for (int i = 0; i < self->internal->numContours; ++i) {
    mcContour_destroy(&self->internal->contours[i]);
  }

  /* Free internal data structure memory */
  free(self->internal->contours);
  free(self->internal);
}

void mcContourBuilder_growContours(mcContourBuilder *self) {
  /* Double the size of the contours buffer */
  mcContour *newContours =
    (mcContour*)malloc(sizeof(mcContour) * self->internal->contoursSize * 2);
  memcpy(
      newContours,
      self->internal->contours,
      sizeof(mcContour) * self->internal->contoursSize);
  free(self->internal->contours);
  self->internal->contours = newContours;
  self->internal->contoursSize *= 2;
  assert(self->internal->numContours < self->internal->contoursSize);
}

const mcContour *mcContourBuilder_contourFromFieldWithArgs(
    mcContourBuilder *self,
    mcScalarFieldWithArgs sf,
    const void *args,
    mcAlgorithmFlag algorithm,
    unsigned int x_res, unsigned int y_res,
    const mcVec2 *min, const mcVec2 *max)
{
  /* Make sure we have enough memory to store this contour */
  if (self->internal->numContours >= self->internal->contoursSize) {
    mcContourBuilder_growContours(self);
  }
  mcContour *contour = &self->internal->contours[self->internal->numContours++];
  mcContour_init(contour);
  /* Extract the contour using the given algorithm */
  switch (algorithm) {
    case MC_MARCHING_SQUARES:
      mcMarchingSquares_contourFromField(
          sf, args,
          x_res, y_res,
          min, max,
          contour);
      break;
    default:
      assert(0);
  }
  return contour;
}
