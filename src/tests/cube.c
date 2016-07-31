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

#include <mc/algorithms/common/cube.h>

int test_mcCube_sampleRelativePosition() {
  int pos[3];
  for (int sampleIndex = 0; sampleIndex < 8; ++sampleIndex) {
    mcCube_sampleRelativePosition(sampleIndex, pos);
    assert(mcCube_sampleIndex(pos[0], pos[1], pos[2]) == sampleIndex);
  }

  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
#define TEST(routine) \
  do { \
    int result; \
    if (result = test_ ## routine()) \
      return result; \
  } while (0)

  TEST(mcCube_sampleRelativePosition);

  return EXIT_SUCCESS;
}
