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

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mcxx/isosurfaceBuilder.h>
#include <mcxx/vector.h>

extern "C" {
#include <mc/algorithms/transvoxel.h>
}

#include "../common/glError.h"
#include "../common/shaderProgram.h"
#include "../common/shaders.h"
#include "interpolatingLattice.h"

#include "transitionObject.h"

namespace mc { namespace samples { namespace transition {
  TransitionObject::TransitionObject(
      int cell,
      const glm::vec3 &position,
      const glm::quat &orientation)
    : MeshObject(position, orientation)
  {
    // Send our buffers to the GL
    m_generateBillboard();
    m_generateTransitionCellWireframe();

    this->setDrawWinding(true);

    this->setCell(cell);
  }

  void TransitionObject::setCell(int cell) {
    m_cell = cell;

    m_update();
  }

  /* XXX: Remove this once we expose transition meshes to the C++ interface */
  float wrapScalarField(
      float x, float y, float z, ScalarField *sf)
  {
    return (*sf)(x, y, z);
  }

  void TransitionObject::m_update() {
    // TODO: Update the transition cell mesh
    auto sf = std::shared_ptr<InterpolatingLattice>(
        new InterpolatingLattice(
          mc::Vec3(0.0f, 0.0f, 0.0f),  // min
          mc::Vec3(1.0f, 1.0f, 1.0f)  // max
          ));
    for (
        int sampleIndex = 0;
        sampleIndex < MC_TRANSVOXEL_NUM_TRANSITION_CELL_SAMPLES;
        ++sampleIndex)
    {
      int pos[2];
      mcTransvoxel_transitionCellSampleRelativePosition(sampleIndex, pos);
      float value = m_cell & (1 << sampleIndex) ? -1.0f : 1.0f;
      for (int i = 0; i < 3; ++i) {
        sf->setSample(pos[0], pos[1], i, value);
      }
    }
    /* XXX
    {
      IsosurfaceBuilder ib;
      auto mesh = ib.buildIsosurface(
          *sf,  // scalar field
          MC_ORIGINAL_MARCHING_CUBES,  // algorithm
          10, 10, 10,  // res
          Vec3(0.0f, 0.0f, 0.0f),  // min
          Vec3(1.0f, 1.0f, 1.0f)  // max
          );
      this->setMesh(*mesh);
    }
    */

    // FIXME: Add a C++ interface for the transvoxel algorithm? Ugh.
    mcMesh mesh;
    mcMesh_init(&mesh);
    mcVec3 min, max;
    min.x = 0.0f;
    min.y = 0.0f;
    min.z = 0.0f;
    max.x = 1.0f;
    max.y = 1.0f;
    max.z = 1.0f;
    mcTransvoxel_transitionMeshFromField(
        (mcScalarFieldWithArgs)wrapScalarField,  // sf
        sf.get(),  // args
        2, 2,  // res
        &min,  // min
        &max,  // max
        &mesh  // mesh
        );
    this->setMesh(mesh);
    mcMesh_destroy(&mesh);
    fprintf(stderr, "mesh->numFaces(): %d\n", mesh.numFaces);
  }

  void TransitionObject::m_generateBillboard() {
    // Make four vertices in a quad arrangement around a center point
    BillboardVertex vertices[] = {
      { .pos = { -1.0f, -1.0f, -1.0f, }, .tex = { 0.0f, 0.0f, }, },
      { .pos = {  1.0f, -1.0f, -1.0f, }, .tex = { 1.0f, 0.0f, }, },
      { .pos = { -1.0f,  1.0f, -1.0f, }, .tex = { 0.0f, 1.0f, }, },
      { .pos = {  1.0f,  1.0f, -1.0f, }, .tex = { 1.0f, 1.0f, }, },
    };
    // Send the vertex data to the GL
    glGenBuffers(1, &m_billboardVertices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_billboardVertices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  // target
        sizeof(vertices),  // size
        vertices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
    // Make two triangles into a quad from our vertices
    unsigned int indices[] = {
      0, 1, 3,
      3, 2, 0,
    };
    // Send the indices to the GL
    glGenBuffers(1, &m_billboardIndices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_billboardIndices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  // target
        sizeof(indices),  // size
        indices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
  }

  void TransitionObject::m_generateTransitionCellWireframe() {
    // Make a vertex on each sample point in the transition cell
    WireframeVertex vertices[] = {
      { .pos = { 0.0f, 0.0f, 0.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 0.5f, 0.0f, 0.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 1.0f, 0.0f, 0.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 0.0f, 0.5f, 0.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 0.5f, 0.5f, 0.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 1.0f, 0.5f, 0.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 0.0f, 1.0f, 0.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 0.5f, 1.0f, 0.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 1.0f, 1.0f, 0.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 0.0f, 0.0f, 1.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 1.0f, 0.0f, 1.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 0.0f, 1.0f, 1.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
      { .pos = { 1.0f, 1.0f, 1.0f, }, .color = { 0.0f, 0.0f, 1.0f, }, },
    };
    // Send the vertex data to the GL
    glGenBuffers(1, &m_transitionCellWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_transitionCellWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  // target
        sizeof(vertices),  // size
        vertices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
    // Make indices for the GL lines
    unsigned int indices[] = {
      0, 2,
      3, 5,
      6, 8,
      0, 6,
      1, 7,
      2, 8,
      0, 9,
      2, 10,
      6, 11,
      8, 12,
      9, 10,
      11, 12,
      9, 11,
      10, 12,
    };
    // Send the indices to the GL
    glGenBuffers(1, &m_transitionCellWireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_transitionCellWireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  // target
        sizeof(indices),  // size
        indices,  // data
        GL_STATIC_DRAW  // usage
        );
    FORCE_ASSERT_GL_ERROR();
  }

  void TransitionObject::m_drawSamplePoint(
      float x, float y, float z,
      float value,
      std::shared_ptr<ShaderProgram> shader,
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
      const glm::mat4 &projection) const
  {
    glm::vec3 pos(x, y, z);
    // We can assume that the billboard point shader is already set up; we just
    // need to make the draw call for the given sample point
    glm::mat4 mw = modelWorld;
    // Translate the sample to its position
    mw = glm::translate(mw, pos);
    // Remove the rotation component of the matrix for proper billboarding
    glm::mat4 mv = worldView * mw;
    mv[0][0] = 1.0f;
    mv[0][1] = 0.0f;
    mv[0][2] = 0.0f;
    mv[1][0] = 0.0f;
    mv[1][1] = 1.0f;
    mv[1][2] = 0.0f;
    mv[2][0] = 0.0f;
    mv[2][1] = 0.0f;
    mv[2][2] = 1.0f;
    mv = glm::scale(mv, glm::vec3(0.08));  // XXX
    // Update the model-view transform uniform
    glUniformMatrix4fv(
        shader->modelViewLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(mv)  // value
        );
    ASSERT_GL_ERROR();
    // Set the color based on the sample value
    float color[3];
    if (value > 0) {
      color[0] = 0.9333f;
      color[1] = 0.3020f;
      color[2] = 0.3686f;
    } else {
      color[0] = 0.4588f;
      color[1] = 0.7961f;
      color[2] = 0.3608f;
    }
    glUniform3f(
        shader->colorLocation(),  // location
        color[0],  // v0
        color[1],  // v1
        color[2]  // v2
        );
    ASSERT_GL_ERROR();
    // Draw the billboarded points
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_billboardIndices);
    ASSERT_GL_ERROR();
    glDrawElements(
        GL_TRIANGLES,  // mode
        6,  // count
        GL_UNSIGNED_INT,  // type
        0  // indices
        );
    ASSERT_GL_ERROR();
  }

  void TransitionObject::m_drawSamplePoints(
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
      const glm::mat4 &projection) const
  {
    // Use the billboard point shader
    auto shader = Shaders::billboardPointShader();
    shader->use();

    // Enable alpha blending
    glEnable(GL_BLEND);
    ASSERT_GL_ERROR();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ASSERT_GL_ERROR();

    // Prepare the uniform values
    assert(shader->modelViewLocation() != -1);
    assert(shader->colorLocation() != -1);
    assert(shader->projectionLocation() != -1);
    glUniformMatrix4fv(
        shader->projectionLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(projection)  // value
        );
    ASSERT_GL_ERROR();

    // Prepare the vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, m_billboardVertices);
    ASSERT_GL_ERROR();
    assert(shader->vertPositionLocation() != -1);
    glEnableVertexAttribArray(shader->vertPositionLocation());
    ASSERT_GL_ERROR();
    glVertexAttribPointer(
        shader->vertPositionLocation(),  // index
        3,  // size
        GL_FLOAT,  // type
        0,  // normalized
        sizeof(BillboardVertex),  // stride
        &(((BillboardVertex *)0)->pos[0])  // pointer
        );
    assert(shader->vertTexCoordLocation() != -1);
    glEnableVertexAttribArray(shader->vertTexCoordLocation());
    ASSERT_GL_ERROR();
    glVertexAttribPointer(
        shader->vertTexCoordLocation(),  // index
        2,  // size
        GL_FLOAT,  // type
        0,  // normalized
        sizeof(BillboardVertex),  // stride
        &(((BillboardVertex *)0)->tex[0])  // pointer
        );
    ASSERT_GL_ERROR();

#define DRAW_SAMPLE_POINT(x, y, z) \
    m_drawSamplePoint( \
        x, y, z, \
        value, \
        shader, \
        modelWorld, \
        worldView, \
        projection)
    for (int i = 0; i < 9; ++i) {
      float value = m_cell & (1 << i) ? -1.0f : 1.0f;
      int pos[2];
      mcTransvoxel_transitionCellSampleRelativePosition(i, pos);
      DRAW_SAMPLE_POINT(
          (float)pos[0] * 0.5f,
          (float)pos[1] * 0.5f,
          0.0f);
      switch (i) {
        case 0:
        case 2:
        case 6:
        case 8:
          DRAW_SAMPLE_POINT(
              (float)pos[0] * 0.5f,
              (float)pos[1] * 0.5f,
              1.0f);
          break;
      }
    }
  }

  void TransitionObject::m_drawTransitionCellWireframe(
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
      const glm::mat4 &projection) const
  {
    // Use the wireframe shader
    auto shader = Shaders::wireframeShader();
    shader->use();

    glm::mat4 modelView = worldView * modelWorld;

    // Prepare the uniform values
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
    glBindBuffer(GL_ARRAY_BUFFER, m_transitionCellWireframeVertices);
    ASSERT_GL_ERROR();
    assert(shader->vertPositionLocation() != -1);
    glEnableVertexAttribArray(shader->vertPositionLocation());
    ASSERT_GL_ERROR();
    glVertexAttribPointer(
        shader->vertPositionLocation(),  // index
        3,  // size
        GL_FLOAT,  // type
        0,  // normalized
        sizeof(WireframeVertex),  // stride
        &(((WireframeVertex *)0)->pos[0])  // pointer
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
        sizeof(WireframeVertex),  // stride
        &(((WireframeVertex *)0)->color[0])  // pointer
        );
    ASSERT_GL_ERROR();

    // Draw the wireframe lines
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_transitionCellWireframeVertices);
    ASSERT_GL_ERROR();
    glDrawElements(
        GL_TRIANGLES,  // mode
        14 * 2,  // count
        GL_UNSIGNED_INT,  // type
        0  // indices
        );
    ASSERT_GL_ERROR();
  }

  void TransitionObject::draw(
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
      const glm::mat4 &projection,
      float alpha, bool debug)
  {
    // Draw a wireframe of the transition cells
    m_drawTransitionCellWireframe(modelWorld, worldView, projection);
    // Draw the sample points in our transition cell
    m_drawSamplePoints(modelWorld, worldView, projection);

    // Draw the mesh itself
    MeshObject::draw(modelWorld, worldView, projection, alpha, debug);
  }
} } }
