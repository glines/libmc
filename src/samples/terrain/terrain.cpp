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
#include <glm/gtc/noise.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <mcxx/scalarField.h>

extern "C" {
#include <mc/algorithms/common/cube.h>
}

#include "../common/camera.h"
#include "../common/glError.h"
#include "../common/shaderProgram.h"
#include "../common/shaders.h"
#include "lodTree.h"
#include "terrainMesh.h"

#include "terrain.h"

namespace mc { namespace samples { namespace terrain {
  float terrain(float x, float y, float z) {
    float mountains = glm::simplex(glm::vec2(x, y) / 5000.0f) * 1000.0f;
    float hills = glm::simplex(glm::vec2(x, y) / 200.0f) * 80.0f;
    float bumps = glm::simplex(glm::vec2(x, y) / 5.0f) * 4.0f;
    return z - (mountains + hills + bumps);
  }

  Terrain::Terrain(std::shared_ptr<Camera> camera)
    : SceneObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::quat()),
    m_camera(camera),
    m_terrainGenerator(terrain)
  {
    m_lastBlock.x = INT_MAX;
    m_lastBlock.y = INT_MAX;
    m_lastBlock.z = INT_MAX;
    // TODO: Prepare a cache of terrain mesh objects
    // TODO: Use an octree structure to organize our terrain meshes

    // TODO: We need a background thread for generating these meshes on the fly.

    // XXX: Experimenting with the LOD tree
    LodTree::Coordinates block;
    /*
    block.x = 0;
    block.y = 0;
    block.z = 0;
    m_lodTree.getNode(block, 0);
    block.x = 1;
    block.y = 1;
    block.z = 1;
    m_lodTree.getNode(block, 0);
    block.x = 42;
    block.y = 13;
    block.z = 90;
    m_lodTree.getNode(block, 0);
    block.x = 42;
    block.y = 13;
    block.z = -90;
    m_lodTree.getNode(block, 0);
    */

    // Send debugging vertices to the GL
    m_generateCubeWireframe();
  }

  void Terrain::tick(float dt) {
    // TODO: Check where the camera is currently and generate terrain around it
    auto cameraPos = m_camera->position();

    m_enqueueTerrain(cameraPos);

    // TODO: Add any recently generated terrain to the scene
    std::shared_ptr<TerrainMesh> mesh;
    while (mesh = m_terrainGenerator.getRecentMesh()) {
      if (!mesh->isEmpty()) {
        this->addChild(mesh);
      }
    }
  }

  void Terrain::draw(const glm::mat4 &modelWorld,
      const glm::mat4 &worldView, const glm::mat4 &projection,
      float alpha, bool debug) const
  {
    // Draw a wireframe representation of the LOD octree
    m_drawLodOctree(modelWorld, worldView, projection);
  }

  void Terrain::m_generateCubeWireframe() {
    // Send a simple wireframe representation of a cube to the GL so that we
    // can draw the octree.
    // Iterate over the cube vertices
    WireframeVertex vertices[8];
    unsigned int pos[3];
    for (int vertex = 0; vertex < 8; ++vertex) {
      mcCube_vertexRelativePosition(vertex, pos);
      vertices[vertex].pos[0] = pos[0] ? 1.0f : 0.0f;
      vertices[vertex].pos[1] = pos[1] ? 1.0f : 0.0f;
      vertices[vertex].pos[2] = pos[2] ? 1.0f : 0.0f;
      vertices[vertex].color[0] = 0.0f;
      vertices[vertex].color[1] = 0.0f;
      vertices[vertex].color[2] = 1.0f;
    }
    // Send the vertices to the GL
    glGenBuffers(1, &m_cubeWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeWireframeVertices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ARRAY_BUFFER,  /* target */
        sizeof(WireframeVertex) * 8,  /* size */
        vertices,  /* data */
        GL_STATIC_DRAW  /* usage */
        );
    FORCE_ASSERT_GL_ERROR();
    // Iterate over cube edges to make edge lines
    unsigned int indices[MC_CUBE_NUM_EDGES * 2];
    for (int edge = 0; edge < MC_CUBE_NUM_EDGES; ++edge) {
      unsigned int vertices[2];
      mcCube_edgeVertices(edge, vertices);
      indices[edge * 2] = vertices[0];
      indices[edge * 2 + 1] = vertices[1];
    }
    // Send the indices to the GL
    glGenBuffers(1, &m_cubeWireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeWireframeIndices);
    FORCE_ASSERT_GL_ERROR();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,  /* target */
        sizeof(unsigned int) * MC_CUBE_NUM_EDGES * 2,  /* size */
        indices,  /* data */
        GL_STATIC_DRAW  /* usage */
        );
    FORCE_ASSERT_GL_ERROR();
  }

  void Terrain::m_drawLodOctree(
      const glm::mat4 &modelWorld,
      const glm::mat4 &worldView,
      const glm::mat4 &projection) const
  {
    // Use our wireframe drawing shader
    auto shader = Shaders::wireframeShader();
    shader->use();
    // Prepare the uniform values
    assert(shader->modelViewLocation() != -1);
    assert(shader->projectionLocation() != -1);
    glUniformMatrix4fv(
        shader->projectionLocation(),  // location
        1,  // count
        0,  // transpose
        glm::value_ptr(projection)  // value
        );
    ASSERT_GL_ERROR();

    // Prepare the vertex attributes
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeWireframeVertices);
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
    // Traverse the LOD octree and draw a cube for each node
    for (auto node : m_lodTree) {
      glm::mat4 mw = modelWorld;
      // Translate the cube wireframe to this node's voxel block position
      mw = glm::translate(mw, node.pos());
      // Scale the cube wireframe according to this node's level of detail
      mw = glm::scale(mw, glm::vec3(node.size()));
      // Update the model-view transform uniform
      glm::mat4 mv = worldView * mw;
      glUniformMatrix4fv(
          shader->modelViewLocation(),  // location
          1,  // count
          0,  // transpose
          glm::value_ptr(mv)  // value
          );
      ASSERT_GL_ERROR();
      // Draw the cube wireframe lines
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeWireframeIndices);
      ASSERT_GL_ERROR();
      glLineWidth(1.0f);
      ASSERT_GL_ERROR();
      glDrawElements(
          GL_LINES,  // mode
          MC_CUBE_NUM_EDGES * 2,  // count
          GL_UNSIGNED_INT,  // type
          0  // indices
          );
      ASSERT_GL_ERROR();
    }
  }

  void Terrain::m_enqueueTerrain(const glm::vec3 &cameraPos) {
    // FIXME: This routine might actually be more at home within the
    // TerrainGenerator class.
    // Determine the voxel block that we are currently in
    LodTree::Coordinates cameraBlock;
    LodTree::posToBlock(cameraPos, &cameraBlock);
    // Check if the voxel block changed
    if (memcmp(&m_lastBlock, &cameraBlock, sizeof(cameraBlock)) == 0)
      return;
    fprintf(stderr, "cameraBlock: (%d, %d, %d)\n",
        cameraBlock.x,
        cameraBlock.y,
        cameraBlock.z);
    memcpy(&m_lastBlock, &cameraBlock, sizeof(cameraBlock));
    // TODO: Determine the delta terrain?
    // TODO: How far out to draw each level of detail? Each level of detail
    // needs to have at least eight meshes generated for it so that it can
    // match up with the next higher level of detail.
    //
    // The camera is located at a specific node in the octree for which we
    // would like to generate the highest level of detail. We also want to
    // generate high level of detail meshes in all nodes adjacent to the node
    // that the camera is in. We query the LOD tree and mark these nodes, 3^3 =
    // 27 in total, as the highest level of detail.
    //
    // Now, for each marked node, we start by generating meshes of the lowest
    // level of detail. Since the lower level of detail meshes are orders of
    // magnitude (by powers of 2) larger than the higher level of detail
    // meshes, most of these low resolution meshes will cover more than one of
    // the 27 voxel blocks we are interested in rendering at the highest
    // resolution.
    //
    // How do we decide the order in which to generate meshes? For each of the
    // 27 high resolution nodes of interest, we submit requests for meshes of
    // each level of detail at that node. Since meshes must be generated in 2^3
    // = 8 mesh blocks, each request for a mesh will mark eight nodes on the
    // octree for generation, although there is much overlap. Each node marked
    // for mesh generation will be placed into a priority heap ordered by least
    // level of detail to highest level of detail.

    // Ensure that terrain around this position is enqueued for generation

    // Iterate over all of the levels of detail we want to generate
    for (int lod = MINIMUM_LOD; lod >= 0; --lod) {
      auto cameraNode = m_lodTree.getNode(cameraBlock, lod);
      // TODO: Iterate over the voxel blocks around the camera
      for (int z = -1; z <= 1; ++z) {
        for (int y = -1; y <= 1; ++y) {
          for (int x = -1; x <= 1; ++x) {
            LodTree::Coordinates offset;
            offset.x = x;
            offset.y = y;
            offset.z = z;
            // XXX: Experiment with the LOD tree
            auto node = m_lodTree.getRelativeNode(*cameraNode, offset);
            // Mark this node for high LOD generation by the terrain
            // generator thread
            m_terrainGenerator.requestDetail(node->block(), lod);
          }
        }
      }
    }
  }
} } }
