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

#ifndef MC_SAMPLES_IMPLICIT_IMPLICIT_SURFACE_H_
#define MC_SAMPLES_IMPLICIT_IMPLICIT_SURFACE_H_

/**
 * \addtogroup samples
 * @{
 */

/**
 * \addtogroup implicit
 * @{
 */

extern "C" {
#include "lua.h"
}

#include <mcxx/isosurfaceBuilder.h>
#include <mcxx/scalarField.h>

#include "../common/meshObject.h"

namespace mc { namespace samples {
  class ShaderProgram;
  namespace implicit {
    /**
     * Represents an implicit surface defined by Lua code. The Lua code to use
     * is passed to the setCode() method.
     */
    class ImplicitSurface : public MeshObject {
      public:
        /**
         * Enumerated list of all scripting languages supported by
         * ImplicitSurface.
         *
         * \todo JAVASCRIPT is not yet supported.
         */
        enum class Language {
          /** Value to use for scripts written in the Lua language. */
          LUA,
          /** Value to use for scripts written in the JavaScript language. */
          JAVASCRIPT,
        };

      private:
        IsosurfaceBuilder m_builder;
        Language m_language;
        std::shared_ptr<ScalarField> m_sf;

        void m_update();

        static const int BUFFER_SIZE = 2048;

        class LuaScalarField : public ScalarField {
          private:
            typedef struct FileAndBuffer {
              FILE *fp;
              char buff[BUFFER_SIZE];
            } FileAndBuffer;

            lua_State *m_lua;
            bool m_valid;

            void m_initLua();
            static const char *m_luaStringReader(
                lua_State *L,
                const char **code,
                size_t *size);
            bool m_readLuaString(const char *code);
            static const char *m_luaFileReader(
                lua_State *L,
                FileAndBuffer *data,
                size_t *size);
            bool m_readLuaFile(FILE *fp);
            void m_closeLua();

            bool m_checkValid();

            static constexpr const char *sfFunction = "sf";

          public:
            LuaScalarField(const char *code);
            LuaScalarField(FILE *fp);
            ~LuaScalarField();

            bool valid() const { return m_valid; }

            float operator()(float x, float y, float z) const;
        };

        const ScalarField &m_scalarField();

      public:
        /**
         * Constructs an implicit isosurface object with the given position and
         * orientation.
         */
        ImplicitSurface(
            const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
            const glm::quat &orientation = glm::quat());

        /**
         * \return The language used scripting with this implicit surface
         * object.
         */
        Language language() { return m_language; }
        /**
         * Set the language used for scripting with this implicit surface
         * object.
         *
         * \param language Flag representing the scripting language to use for
         * writing scripts with this object.
         */
        void setLanguage(Language language);

        /**
         * Sets the code for the implicit isosurface function. Returns true
         * upon success, and false otherwise.
         *
         * \param code Null-terminated string containing the entire code for
         * the script to run.
         * \return True upon success, false otherwise.
         *
         * If the code is invalid or otherwise fails to run, then the implicit
         * isosurface function remains unchanged.
         */
        bool setCode(const char *code);

        /**
         * Sets the code for the implicit isosurface function. Returns true
         * upon success, and false otherwise.
         *
         * \param fp Opened file stream of file containing all of the code for
         * this script.
         * \return True upon success, false otherwise.
         *
         * If the code is invalid or otherwise fails to run, then the implicit
         * isosurface function remains unchanged.
         */
        bool setCode(FILE *fp);
    };
  }
} }

/** @} */

/** @} */

#endif
