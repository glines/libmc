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
  /* Free internal data structure memory */
  free(self->internal->contours);
  free(self->internal);
}

const mcContour *mcContourBuilder_contourFromFieldWithArgs(
    mcContourBuilder *self,
    mcScalarFieldWithArgs sf,
    const void *args,
    mcAlgorithmFlag algorithm,
    unsigned int x_res, unsigned int y_res,
    const mcVec2 *min, const mcVec2 *max)
{
  // TODO
}
