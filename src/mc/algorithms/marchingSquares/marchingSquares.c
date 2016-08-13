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

#include <mc/algorithms/common/square.h>

void mcMarchingSquares_isolineFromField(
    mcScalarFieldWithArgs sf, const void *args,
    unsigned int x_res, unsigned int y_res,
    const mcVec3 *min, const mcVec3 *max,
    mcLines *lines)
{
  /* TODO: Make a cache of sample values */
  /* Loop over the sample lattice */
  for (int y = 0; y < y_res - 1; ++y) {
    for (int x = 0; x < x_res - 1; ++x) {
      /* TODO: Determine the configuration of this square */
      int square = 0;
      for (int sampleIndex = 0; sampleIndex < 4; ++sampleIndex) {
        int pos[2];
        mcSquare_sampleRelativePosition(i, pos);
        /* TODO: Retrieve the sample value from a buffer */
        float sample = sf(min->x + (float)(x + pos[0]) * delta_x,
                          min->y + (float)(y + pos[1]) * delta_y,
                          0.0f,
                          args);
        square |= (samples >= 0.0f ? 0 : 1) << sampleIndex;
      }
      fprintf(stderr, "square: 0x%01x\n", square);
    }
  }
}
