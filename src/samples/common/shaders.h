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

#ifndef MC_SAMPLES_COMMON_SHADERS_H_
#define MC_SAMPLES_COMMON_SHADERS_H_

/**
 * \addtogroup samples
 * @{
 */

/**
 * \addtogroup common
 * @{
 */

#include <memory>

namespace mc { namespace samples {
  class ShaderProgram;
  /**
   * Contains static declarations for GL shaders that are used in various
   * sample programs. The GLSL source code for these shaders can be found in
   * the src/samples/common/assets/shaders directory.
   *
   * These shaders used to be read from files, but now the GLSL source code is
   * compiled into a C string within the samples_common static library. While
   * this is somewhat less flexible, it allows many of the sample programs to
   * run without any runtime file dependencies.
   */
  class Shaders {
    public:
#define DECLARE_SHADER(shader) \
      static std::shared_ptr<ShaderProgram> shader ## Shader()

      /** Shader for drawing quad billboards. */
      DECLARE_SHADER(billboard);
      /** Shader for drawing points as billboards (since WebGL doesn't support
       * point sprites). */
      DECLARE_SHADER(billboardPoint);
      /** Shader implementing Gouraud shading. */
      DECLARE_SHADER(gouraud);
      /** Shader for drawing debugging points. */
      DECLARE_SHADER(point);
      /** Shader for drawing debugging lines. */
      DECLARE_SHADER(wireframe);
  };
} }

/** @} */

/** @} */

#endif
