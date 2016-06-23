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

#include <string.h>

#include <mc/algorithms.h>

mcAlgorithmFlag mcAlgorithm_stringToFlag(const char *string) {
  typedef struct StringFlagPair {
    const char *string;
    mcAlgorithmFlag flag;
  } StringFlagPair;
#define STRING_FLAG(a) \
  { .string = "MC_" #a, .flag = MC_ ## a, }
  static const StringFlagPair table[] = {
    STRING_FLAG(UNKNOWN_ALGORITHM),
    STRING_FLAG(DEFAULT_ALGORITHM),
    STRING_FLAG(CPU_PERFORMANCE_ALGORITHM),
    STRING_FLAG(CPU_BALANCE_ALGORITHM),
    STRING_FLAG(CPU_QUALITY_ALGORITHM),
    STRING_FLAG(GPGPU_QUALITY_ALGORITHM),
    STRING_FLAG(GPGPU_BALANCE_ALGORITHM),
    STRING_FLAG(LOW_MEMORY_ALGORITHM),
    STRING_FLAG(SIMPLE_MARCHING_CUBES),
    STRING_FLAG(DUAL_MARCHING_CUBES),
    STRING_FLAG(ELASTIC_SURFACE_NETS),
    STRING_FLAG(CUBERILLE),
    STRING_FLAG(SNAP_MARCHING_CUBES),
    STRING_FLAG(PATCH_MARCHING_CUBES),
    STRING_FLAG(MIDPOINT_MARCHING_CUBES),
    STRING_FLAG(NIELSON_DUAL),
    STRING_FLAG(ORIGINAL_MARCHING_CUBES),
  };
  for (int i = 0; i < sizeof(table) / sizeof(table[0]); ++i) {
    if (strcmp(string, table[i].string) == 0) {
      return table[i].flag;
    }
  }
  return MC_UNKNOWN_ALGORITHM;
}
