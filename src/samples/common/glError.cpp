#include <cstdio>

#include "glError.h"

bool checkGlError(const char *file, int line) {
  bool result = false;
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    fprintf(stderr, "GL error: '%s'",
        glErrorToString(error));
    result = true;
  }
  return result;
}

const char *glErrorToString(GLenum error) {
// FIXME: Rename this macro
#define GL_E2S(error) \
  case GL_ ## error: \
                     return "GL_" #error;

  switch (error) {
    // From: <https://www.opengl.org/wiki/OpenGL_Error#Meaning_of_errors>
#ifdef GL_INVALID_ENUM
    GL_E2S(INVALID_ENUM)
#endif
#ifdef GL_INVALID_VALUE
      GL_E2S(INVALID_VALUE)
#endif
#ifdef GL_INVALID_OPERATION
      GL_E2S(INVALID_OPERATION)
#endif
#ifdef GL_STACK_OVERFLOW
      GL_E2S(STACK_OVERFLOW)
#endif
#ifdef GL_STACK_UNDERFLOW
      GL_E2S(STACK_UNDERFLOW)
#endif
#ifdef GL_OUT_OF_MEMORY
      GL_E2S(OUT_OF_MEMORY)
#endif
#ifdef GL_INVALID_FRAMEBUFFER_OPERATION
      GL_E2S(INVALID_FRAMEBUFFER_OPERATION)
#endif
#ifdef GL_CONTEXT_LOST
      GL_E2S(CONTEXT_LOST)
#endif
#ifdef GL_TABLE_TOO_LARGE
      GL_E2S(TABLE_TOO_LARGE)
#endif
    default:
      return "UNKNOWN";
  }
}
