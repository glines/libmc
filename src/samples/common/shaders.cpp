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

#include "shaderProgram.h"
#include "shaders.h"

namespace mc { namespace samples {
#ifdef __EMSCRIPTEN__
#define SHADER_DIR assets_shaders_webgl
#else
#define SHADER_DIR assets_shaders_glsl
#endif

#define CAT(a, b) a ## b

#define DEFINE_SHADER_BASE(shader, dir) \
  std::shared_ptr<ShaderProgram> Shaders:: shader ## Shader() { \
    static std::shared_ptr<ShaderProgram> instance = \
      std::shared_ptr<ShaderProgram>( \
          new ShaderProgram( \
            (const char *)CAT(dir, _ ## shader ## _vert), \
            CAT(dir, _ ## shader ## _vert_len), \
            (const char *)CAT(dir, _ ## shader ## _frag), \
            CAT(dir, _ ## shader ## _frag_len) \
            )); \
    return instance; \
  }
#define DEFINE_SHADER(shader) DEFINE_SHADER_BASE(shader, SHADER_DIR)

#ifdef __EMSCRIPTEN__
#include "assets_shaders_webgl_billboard.vert.c"
#include "assets_shaders_webgl_billboard.frag.c"
#else
#include "assets_shaders_glsl_billboard.vert.c"
#include "assets_shaders_glsl_billboard.frag.c"
#endif
  DEFINE_SHADER(billboard)
#ifdef __EMSCRIPTEN__
#include "assets_shaders_webgl_billboardPoint.vert.c"
#include "assets_shaders_webgl_billboardPoint.frag.c"
#else
#include "assets_shaders_glsl_billboardPoint.vert.c"
#include "assets_shaders_glsl_billboardPoint.frag.c"
#endif
  DEFINE_SHADER(billboardPoint)
#ifdef __EMSCRIPTEN__
#include "assets_shaders_webgl_gouraud.vert.c"
#include "assets_shaders_webgl_gouraud.frag.c"
#else
#include "assets_shaders_glsl_gouraud.vert.c"
#include "assets_shaders_glsl_gouraud.frag.c"
#endif
  DEFINE_SHADER(gouraud)
#ifdef __EMSCRIPTEN__
#include "assets_shaders_webgl_point.vert.c"
#include "assets_shaders_webgl_point.frag.c"
#else
#include "assets_shaders_glsl_point.vert.c"
#include "assets_shaders_glsl_point.frag.c"
#endif
  DEFINE_SHADER(point)
#ifdef __EMSCRIPTEN__
#include "assets_shaders_webgl_wireframe.vert.c"
#include "assets_shaders_webgl_wireframe.frag.c"
#else
#include "assets_shaders_glsl_wireframe.vert.c"
#include "assets_shaders_glsl_wireframe.frag.c"
#endif
  DEFINE_SHADER(wireframe)
} }
