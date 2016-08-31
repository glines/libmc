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

#ifndef MC_CONTOUR_BUILDER_H_
#define MC_CONTOUR_BUILDER_H_

#include <mc/algorithms.h>
#include <mc/coloredField.h>
#include <mc/contour.h>
#include <mc/scalarField.h>

typedef struct mcContourBuilderInternal mcContourBuilderInternal;

typedef struct {
  mcContourBuilderInternal *internal;
} mcContourBuilder;

void mcContourBuilder_init(mcContourBuilder *self);

void mcContourBuilder_destroy(mcContourBuilder *self);

const mcContour *mcContourBuilder_contourFromFieldWithArgs(
    mcContourBuilder *self,
    mcScalarFieldWithArgs sf,
    const void *args,
    mcAlgorithmFlag algorithm,
    unsigned int x_res, unsigned int y_res,
    const mcVec2 *min, const mcVec2 *max);

const mcContour *mcContourBuilder_contourFromColoredFieldWithArgs(
    mcContourBuilder *self,
    mcColoredFieldWithArgs cf,
    const void *args,
    mcAlgorithmFlag algorithm,
    unsigned int x_res, unsigned int y_res,
    const mcVec2 *min, const mcVec2 *max);

#endif
