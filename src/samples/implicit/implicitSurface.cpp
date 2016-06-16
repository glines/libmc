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

extern "C" {
#include "lauxlib.h"
#include "lualib.h"
}

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <mcxx/vector.h>

#include "implicitSurface.h"

namespace mc { namespace samples { namespace implicit {
  ImplicitSurface::ImplicitSurface(
      const glm::vec3 &position,
      const glm::quat &orientation)
    : MeshObject(position, orientation),
      m_language(ImplicitSurface::Language::LUA)
  {
    this->setDrawOpaque(false);
  }

  void ImplicitSurface::m_update() {
    /* This method is called after the code for the implicit surface is
     * updated, or when some parameter that affects how the isosurface mesh is
     * extracted is changed. */

    // Re-evaluate the isosurface extraction algorithm
    const Mesh *mesh = m_builder.buildIsosurface(
        *m_sf,  // scalarField
        MC_NIELSON_DUAL,  // algorithm
        20, 20, 20,  // res
        Vec3(-1.0f, -1.0f, -1.0f),  // min
        Vec3(1.0f, 1.0f, 1.0f)  // max
        );

    // Generate wireframe data and send it to the GL
    this->setMesh(*mesh);
  }

  bool ImplicitSurface::setCode(const char *code) {
    bool success = false;
    switch (m_language) {
      case Language::LUA:
        {
          /* Load the Lua code from the given string */
          LuaScalarField *sf = new LuaScalarField(code);
          if (sf->valid()) {
            m_sf = std::shared_ptr<ScalarField>(sf);
            success = true;
          } else {
            delete sf;
          }
        }
        break;
      case Language::JAVASCRIPT:
        fprintf(stderr, "Javascript language not yet supported\n");
        assert(false);
        break;
    }

    if (success) {
      /* Update the isosurface now that the implicit function has changed */
      m_update();
    }

    return success;
  }

  bool ImplicitSurface::setCode(FILE *fp) {
    /* Read the entire file into a C string and call the other method. */
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    rewind(fp);
    char *code = (char*)malloc(length+1);
    length = fread(code, 1, length, fp);
    code[length] = '\0';
    fprintf(stderr, "code: %s\n", code);
    bool result = this->setCode(code);
    free(code);
    return result;
  }

  ImplicitSurface::LuaScalarField::LuaScalarField(const char *code) {
    m_initLua();
    if (m_readLuaString(code))
      m_valid = m_checkValid();
    else
      m_valid = false;
  }

  ImplicitSurface::LuaScalarField::LuaScalarField(FILE *fp) {
    // XXX: I don't think this code is being used
    m_initLua();
    if (m_readLuaFile(fp))
      m_valid = m_checkValid();
    else
      m_valid = false;
  }

  ImplicitSurface::LuaScalarField::~LuaScalarField() {
    m_closeLua();
  }

  void ImplicitSurface::LuaScalarField::m_initLua() {
    // Initialize the Lua state object
    m_lua = luaL_newstate();
    if (m_lua == nullptr) {
      fprintf(stderr, "Failed to allocate a new Lua state\n");
      exit(EXIT_FAILURE);
    }
    /*
    // Open the math library
    luaL_requiref(m_lua,
        "math",  // modname
        luaopen_math,  // openf
        1  // global
        );
    lua_pop(m_lua, 1);
    */
    luaL_openlibs(m_lua);
  }

  const char *ImplicitSurface::LuaScalarField::m_luaStringReader(
      lua_State *L,
      const char **code,
      size_t *size)
  {
    if (*code != nullptr) {
      *size = strlen(*code);
      const char *result = *code;
      *code = nullptr;
      return result;
    }
    return 0;
  }

  bool ImplicitSurface::LuaScalarField::m_readLuaString(const char *code) {
    luaL_loadstring(m_lua, code);
    int result = lua_pcall(m_lua, 0, 0, 0);
    if (result != LUA_OK) {
      fprintf(stderr, "Failed to read Lua string.\n");
      return false;
    }
    return true;
  }

  const char *ImplicitSurface::LuaScalarField::m_luaFileReader(
      lua_State *L,
      FileAndBuffer *data,
      size_t *size)
  {
    *size = fread(data->buff, sizeof(char), BUFFER_SIZE, data->fp);
    return data->buff;
  }

  bool ImplicitSurface::LuaScalarField::m_readLuaFile(FILE *fp) {
    FileAndBuffer data;
    data.fp = fp;
    lua_load(m_lua,
        (lua_Reader)m_luaFileReader,  // reader
        &data,  // data
        "ImplicitSurface::ScalarField::m_readLuaFile",  // source
        nullptr  // mode
        );
    int result = lua_pcall(m_lua, 0, 0, 0);
    if (result != LUA_OK) {
      fprintf(stderr, "Failed to call Lua file.\n");
      return false;
    }
    return true;
  }

  void ImplicitSurface::LuaScalarField::m_closeLua() {
    // Close the Lua state
    lua_close(m_lua);
  }

  bool ImplicitSurface::LuaScalarField::m_checkValid() {
    // Look for the scalar field function in the global scope
    lua_getglobal(m_lua, sfFunction);
    if (!lua_isfunction(m_lua, -1)) {
      fprintf(stderr, "The scalar field function '%s' was not defined in Lua\n",
          sfFunction);
      lua_pop(m_lua, 1);
      return false;
    }
    // Call the function and check for valid output
    lua_pushnumber(m_lua, 0.0);
    lua_pushnumber(m_lua, 0.0);
    lua_pushnumber(m_lua, 0.0);
    lua_call(m_lua, 3, 1);
    if (!lua_isnumber(m_lua, -1)) {
      fprintf(stderr, "The scalar field function '%s' did not return a number",
          sfFunction);
      lua_pop(m_lua, 1);
      return false;
    }
    lua_pop(m_lua, 1);
    return true;
  }

  float ImplicitSurface::LuaScalarField::operator()(
      float x, float y, float z) const
  {
    // Pass our three arguments and call the scalar field function
    lua_getglobal(m_lua, sfFunction);
    if (!lua_isfunction(m_lua, -1)) {
      fprintf(stderr, "The scalar field function '%s' was not defined in Lua\n",
          sfFunction);
      lua_pop(m_lua, 1);
      return 0.0f;
    }
    lua_pushnumber(m_lua, x);
    lua_pushnumber(m_lua, y);
    lua_pushnumber(m_lua, z);
    lua_call(m_lua, 3, 1);
    if (!lua_isnumber(m_lua, -1)) {
      fprintf(stderr, "The scalar field function '%s' did not return a number",
          sfFunction);
      lua_pop(m_lua, 1);
      return 0.0f;
    }
    float result = lua_tonumber(m_lua, -1);
    lua_pop(m_lua, 1);
    return result;
  }
} } }
