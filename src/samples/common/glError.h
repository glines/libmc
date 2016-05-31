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

#ifndef MC_SAMPLES_COMMON_GL_ERROR_H_
#define MC_SAMPLES_COMMON_GL_ERROR_H_

#include <cassert>

#include <GL/glew.h>

#ifdef __EMSCRIPTEN__
#define DISABLE_CHECK_GL_ERROR true
#else
#define DISABLE_CHECK_GL_ERROR false
#endif

#define FORCE_CHECK_GL_ERROR() \
  (checkGlError(__FILE__, __LINE__))

#define FORCE_ASSERT_GL_ERROR() \
  if (checkGlError(__FILE__, __LINE__)) \
    assert(false);

#if DISABLE_CHECK_GL_ERROR

#define CHECK_GL_ERROR() (false)

#define ASSERT_GL_ERROR() ;

#else  // DISABLE_CHECK_GL_ERROR

#define CHECK_GL_ERROR() \
  (checkGlError(__FILE__, __LINE__))

#define ASSERT_GL_ERROR() \
  if (checkGlError(__FILE__, __LINE__)) \
    assert(false);

#endif  // DISABLE_CHECK_GL_ERROR

bool checkGlError(const char *file, int line);
// FIXME: Rename this function?
const char *glErrorToString(GLenum error);

#endif
