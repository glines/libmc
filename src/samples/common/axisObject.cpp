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

#include <glm/gtc/type_ptr.hpp>

#include "glError.h"
#include "shaderProgram.h"
#include "shaders.h"

#include "axisObject.h"

namespace mc { namespace samples {
  AxisObject::AxisObject(
      const glm::vec3 &position, const glm::quat &orientation)
    : SceneObject(position, orientation)
  {
    // Send three lines representing the axis to the GL
    Vertex vertices[] = {
      { .pos = { 0.0f, 0.0f, 0.0f, }, .color = { 1.0f, 0.0f, 0.0f, }, },
      { .pos = { 1.0f, 0.0f, 0.0f, }, .color = { 1.0f, 0.0f, 0.0f, }, },
      { .pos = { 0.0f, 0.0f, 0.0f, }, .color = { 0.0f, 1.0f, 0.0f, }, },
      { .pos = { 0.0f, 1.0f, 0.0f, }, .color = { 0.0f, 1.0f, 0.0f, }, },
      { .pos = { 0.0f, 0.0f, 0.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 0.0f, 0.0f, 1.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
    };
    glGenBuffers(1, &m_vertexBuffer);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  // target
        sizeof(vertices),  // size
        vertices,  // data
        GL_STATIC_DRAW  // hint
        );
    FORCE_ASSERT_GL_ERROR();
  }

  void AxisObject::draw(const glm::mat4 &modelWorld,
      const glm::mat4 &worldView, const glm::mat4 &projection,
      float alpha, bool debug)
  {
    auto shader = Shaders::wireframeShader();
    shader->use();

    glm::mat4 modelView = worldView * modelWorld;

    // TODO: Prepare the uniform values
    assert(shader->modelViewLocation() != -1);
    glUniformMatrix4fv(
        shader->modelViewLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(modelView)  // value
        );
    ASSERT_GL_ERROR();
    assert(shader->projectionLocation() != -1);
    glUniformMatrix4fv(
        shader->projectionLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(projection)  // value
        );
    ASSERT_GL_ERROR();

    // Prepare the vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    ASSERT_GL_ERROR();
    assert(shader->vertPositionLocation() != -1);
    glEnableVertexAttribArray(shader->vertPositionLocation());
    ASSERT_GL_ERROR();
    glVertexAttribPointer(
        shader->vertPositionLocation(),  // index
        3,  // size
        GL_FLOAT,  // type
        0,  // normalized
        sizeof(Vertex),  // stride
        &(((Vertex *)0)->pos[0])  // pointer
        );
    ASSERT_GL_ERROR();
    assert(shader->vertColorLocation() != -1);
    glEnableVertexAttribArray(shader->vertColorLocation());
    ASSERT_GL_ERROR();
    glVertexAttribPointer(
        shader->vertColorLocation(),  // index
        3,  // size
        GL_FLOAT,  // type
        0,  // normalized
        sizeof(Vertex),  // stride
        &(((Vertex *)0)->color[0])  // pointer
        );
    ASSERT_GL_ERROR();

    // Draw the axis lines
    glLineWidth(5.0f);
    ASSERT_GL_ERROR();
    glDrawArrays(
        GL_LINES,  // mode
        0,  // first
        3 * 2  // count
        );
    ASSERT_GL_ERROR();
  }

} }
