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
