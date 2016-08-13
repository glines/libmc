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

#ifndef MC_SAMPLES_COMMON_TEMPLATE_H_
#define MC_SAMPLES_COMMON_TEMPLATE_H_

#include <string>

extern "C" {
#include "lua.h"
}

/**
 * \addtogroup samples
 * @{
 */

/**
 * \addtogroup common
 * @{
 */

namespace mc { namespace samples {
  /**
   * Class providing a Lua template engine for opening files augmented with a
   * template language.
   *
   * This class is most notably used to apply templates to GLSL shader
   * programs, which command a large number of options across both native GL
   * and WebGL.
   */
  class Template {
    private:
      static lua_State *m_lua();
      static void m_initLua(lua_State **lua);
    public:
      /**
       * Constructs a templatized version of the file at the given file path.
       *
       * \param file Path to a file written with our Lua template syntax.
       */
      Template(const std::string &file);
  };
} }

/** @} */

/** @} */

#endif
