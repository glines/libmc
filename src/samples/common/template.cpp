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

#include <cstdlib>

extern "C" {
#include "lauxlib.h"
}

#include "template.h"

namespace mc { namespace samples {
  Template::Template(const std::string &file) {
    // TODO: Initialize the Lua state
    // TODO: Load our template engine
    // TODO: Read the file from 
  }

  lua_State *Template::m_lua() {
    static lua_State *instance = nullptr;
    if (instance == nullptr) {
      m_initLua(&instance);
    }
    return instance;
  }

  void Template::m_initLua(lua_State **lua) {
    // Initialize the Lua state object
    *lua = luaL_newstate();
    if (*lua == nullptr) {
      fprintf(stderr, "Failed to allocate a new Lua state\n");
      exit(EXIT_FAILURE);
    }
    // Load the template engine from file
    const char *enginePath = "./assets/lua/templateEngine.lua";
    int result;
    result = luaL_loadfile(*lua, enginePath);
    if (result != LUA_OK) {
      fprintf(stderr, "Failed to open Lua template engine '%s'\n",
          enginePath);
      exit(EXIT_FAILURE);
    }
  }
} }
