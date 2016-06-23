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

#ifndef MC_SAMPLES_COMMON_SHADER_PROGRAM_H_
#define MC_SAMPLES_COMMON_SHADER_PROGRAM_H_

#include <GL/glew.h>
#include <string>

namespace mc { namespace samples {
  /**
   * Class representing a compiled and linked GL shader program, suitable for
   * rendering GL graphics.
   */
  class ShaderProgram {
    private:
      // FIXME: Clean these up with a couple macros
      // FIXME: Remove "Location" from all of these variables
      GLuint m_shaderProgram;
      GLuint m_modelViewLocation, m_projectionLocation,
             m_modelViewProjectionLocation, m_normalTransformLocation,
             m_lightPositionLocation, m_lightIntensityLocation,
             m_timeLocation;
      GLuint m_vertPositionLocation, m_vertNormalLocation, m_vertColorLocation,
             m_vertTexCoordLocation, m_vertVelocityLocation;
      GLuint m_vertStartTimeLocation;
      GLuint m_texture0;

      static GLuint m_compileShader(
          const char *code, int code_len, GLenum type);
      static void m_linkShaderProgram(GLuint shaderProgram);

      void m_init(
          const char *vert, unsigned int vert_len,
          const char *frag, unsigned int frag_len);
      void m_readCode(const std::string &path, char **code, unsigned int *code_len);

    protected:
      /**
       * A virtual member function to allow derived classes the opportunity to
       * register attribute and/or uniform locations immediately after the shader
       * program has been linked.
       */
      // FIXME: This virtual call cannot be made in the constructor... I should
      //        just remove this.
      virtual void initLocations();

    public:
      /**
       * Constructs an OpenGL shader program compiled and linked from the given
       * vertex and fragment shaders.
       *
       * \param vert Path to the vertex shader to compile.
       * \param frag Path to the fragment shader to compile.
       */
      ShaderProgram(const std::string &vert, const std::string &frag);
      /**
       * Constructs an OpenGL shader program compiled and linked directly from
       * string given for the vertex and fragment shader source.
       *
       * \param vert Source code string of the vertex shader to compile.
       * \param vert_len Length of the vertex shader source code string.
       * \param frag Source code string of the vertex shader to compile.
       * \param frag_len Length of the vertex shader source code string.
       */
      ShaderProgram(
          const char *vert, unsigned int vert_len,
          const char *frag, unsigned int frag_len);
      /**
       * Destroy this shader program.
       *
       * \todo Free the GL resources associated with ShaderProgram upon
       * destruction.
       */
      virtual ~ShaderProgram();

      /**
       * \return Location of the model-view transform matrix uniform in the
       * shader.
       */
      GLuint modelViewLocation() const { return m_modelViewLocation; }
      /**
       * \return Location of the projection transform matrix uniform in the
       * shader.
       */
      GLuint projectionLocation() const { return m_projectionLocation; }
      /**
       * \return Location of the combination model-view-projection transform
       * matrix uniform in the shader.
       */
      GLuint modelViewProjectionLocation() const { return m_modelViewProjectionLocation; }
      /**
       * \return Location of the normal transform matrix for transforming
       * surface normals into view space.
       *
       * \todo Does the normal transform go from model to view space? I don't
       * remember.
       */
      GLuint normalTransformLocation() const { return m_normalTransformLocation; }
      /**
       * \return Location of the point light position uniform in the shader.
       *
       * \todo Replace single point light location with an array of structs.
       */
      GLuint lightPositionLocation() const { return m_lightPositionLocation; }
      /**
       * \return Location of the point light intensity uniform in the shader.
       */
      GLuint lightIntensityLocation() const { return m_lightIntensityLocation; }
      /**
       * \return Location of the current time uniform in the shader.
       */
      GLuint timeLocation() const { return m_timeLocation; }
      /**
       * \return Location of the vertex position vertex attribute in the
       * shader.
       */
      GLuint vertPositionLocation() const { return m_vertPositionLocation; }
      /**
       * \return Location of the vertex normal vertex attribute in the
       * shader.
       */
      GLuint vertNormalLocation() const { return m_vertNormalLocation; }
      /**
       * \return Location of the vertex color vertex attribute in the shader.
       * This is used primarily for specifying the color for drawing debugging
       * points and lines.
       */
      GLuint vertColorLocation() const { return m_vertColorLocation; }
      /**
       * \return Location of the texture coordinate vertex attribute in the
       * shader.
       */
      GLuint vertTexCoordLocation() const { return m_vertTexCoordLocation; }
      /**
       * \return Location of the vertex velocity attribute in the shader.
       *
       * \todo This probably is not being used. Remove the vertVelocity
       * attribute from ShaderProgram.
       */
      GLuint vertVelocityLocation() const { return m_vertVelocityLocation; }
      /**
       * \return Location of the start time uniform in the shader.
       *
       * \todo This probably is not being used. Remove the vertStartTime
       * attribute from ShaderProgram.
       */
      GLuint vertStartTimeLocation() const { return m_vertStartTimeLocation; }
      /**
       * \return Location of the first texture sampler uniform in the shader.
       */
      GLuint texture0() const { return m_texture0; }

      /**
       * Use this shader in the current GL state.
       */
      void use() const;
  };
} }

#endif
