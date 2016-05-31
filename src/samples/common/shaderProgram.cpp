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

#include <cstdio>
#include <cstdlib>

#include "shaderProgram.h"

namespace mc { namespace samples {
  ShaderProgram::ShaderProgram(
      const std::string &vert, const std::string &frag)
  {
    m_shaderProgram = glCreateProgram();

    GLuint vertexShader = -1;
    if (!vert.empty()) {
      vertexShader = m_compileShader(vert, GL_VERTEX_SHADER);
      glAttachShader(m_shaderProgram, vertexShader);
    }
    GLuint fragmentShader = -1;
    if (!frag.empty()) {
      fragmentShader = m_compileShader(frag, GL_FRAGMENT_SHADER);
      glAttachShader(m_shaderProgram, fragmentShader);
    }

    m_linkShaderProgram(m_shaderProgram);

    initLocations();
  }

  ShaderProgram::~ShaderProgram() {
  }

  void ShaderProgram::use() const {
    glUseProgram(m_shaderProgram);
  }

  GLuint ShaderProgram::m_compileShader(const std::string &file, GLenum type) {
    FILE *f;
    int length;
    char *source;
    GLint status;

    GLuint shader = glCreateShader(type);

    f = fopen(file.c_str(), "r");
    if (f == NULL) {
      fprintf(stderr, "Could not open shader file: %s\n", file.c_str());
      exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    rewind(f);
    source = (char*)malloc(length+1);
    length = fread(source, 1, length, f);
    source[length] = '\0';
    glShaderSource(shader, 1, (const char **)&source, &length);
    free(source);

    glCompileShader(shader);
    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &status);
    if (status != GL_TRUE) {
      char *log;
      int logLength;
      glGetShaderiv(
          shader,
          GL_INFO_LOG_LENGTH,
          &logLength);
      log = (char*)malloc(logLength);
      glGetShaderInfoLog(
          shader,
          logLength,
          NULL,
          log);
      fprintf(stderr, "Error compiling shader '%s': %s\n",
          file.c_str(), log);
      free(log);
      // FIXME: Maybe call something other than exit() here
      exit(EXIT_FAILURE);
    }

    return shader;
  }

  void ShaderProgram::m_linkShaderProgram(GLuint shaderProgram) {
    GLint status;

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
      char *log;
      int logLength;
      glGetProgramiv(
          shaderProgram,
          GL_INFO_LOG_LENGTH,
          &logLength);
      log = (char *)malloc(logLength);
      glGetProgramInfoLog(
          shaderProgram,
          logLength,
          NULL,
          log);
      fprintf(stderr, "Error linking shader program: %s\n", log);
      free(log);
      // FIXME: Maybe call something other than exit() here
      exit(EXIT_FAILURE);
    }
  }

  void ShaderProgram::initLocations() {
    // Register some common uniform and attribute locations
    m_modelViewLocation = glGetUniformLocation(
        m_shaderProgram, "modelView");
    m_projectionLocation = glGetUniformLocation(
        m_shaderProgram, "projection");
    m_timeLocation = glGetUniformLocation(
        m_shaderProgram, "time");

    m_vertPositionLocation = glGetAttribLocation(
        m_shaderProgram, "vertPosition");
    m_vertNormalLocation = glGetAttribLocation(
        m_shaderProgram, "vertNormal");
    m_vertColorLocation = glGetAttribLocation(
        m_shaderProgram, "vertColor");
    m_vertTexCoordLocation = glGetAttribLocation(
        m_shaderProgram, "vertTexCoord");
    m_vertVelocityLocation = glGetAttribLocation(
        m_shaderProgram, "vertVelocity");
    m_vertStartTimeLocation = glGetAttribLocation(
        m_shaderProgram, "vertStartTime");

    m_texture0 = glGetUniformLocation(
        m_shaderProgram, "texture0");
  }
} }
