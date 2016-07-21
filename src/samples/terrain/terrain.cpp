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
#include "generateTerrainTask.h"
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

  float wave(float x, float y, float z) {
    float interval = 800.0f;
    float amplitude = 1000.0f;
    return z - (cos(x / interval) * sin(y / interval)) * amplitude;
  }

  float hills(float x, float y, float z) {
    float interval = 800.0f;
    float amplitude = 1000.0f;
#define ADD_HILLS(interval,amplitude) \
    ((cos(x / interval) * sin(y / interval)) * amplitude)
#define ADD_RANDOM_HILLS(interval,amplitude) \
    (( \
      cos(x / (interval + glm::simplex(glm::vec2(x, y)))) \
      * sin(y / (interval + 0.3f * glm::simplex(glm::vec2(x, y))))) * amplitude)
    return z -
      (ADD_HILLS(800.0f, 100.0f)
       + ADD_HILLS(72.0f, 25.0f)
       + ADD_HILLS(40.0f, 50.0f));
  }

  Terrain::Terrain(std::shared_ptr<Camera> camera, int minimumLod)
    : SceneObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::quat()),
    m_camera(camera),
    m_sf(hills),
    m_workers(std::thread::hardware_concurrency()),
    m_minimumLod(minimumLod)
  {
    m_lastCameraBlock.x = INT_MAX;
    m_lastCameraBlock.y = INT_MAX;
    m_lastCameraBlock.z = INT_MAX;

    // Send debugging vertices to the GL
    m_generateCubeWireframe();
  }

  void Terrain::tick(float dt) {
    m_updateCamera();

    // Inform nodes of recently generated meshes
    RecentMesh recentMesh;
    while ((recentMesh = m_getRecentMesh()).mesh) {
      m_handleNewMesh(recentMesh.mesh, recentMesh.node);
    }
  }

  void Terrain::draw(const glm::mat4 &modelWorld,
      const glm::mat4 &worldView, const glm::mat4 &projection,
      float alpha, bool debug)
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

  void Terrain::m_updateCamera() {
    // TODO: Add some logic to Terrain::m_updateCamera() that considers the
    // direction the camera is facing and the viewing angle of the camera's
    // projection.

    // Check if the camera moved to a different voxel block
    LodTree::Coordinates cameraBlock;
    LodTree::posToBlock(m_camera->position(), &cameraBlock);
    if (memcmp(&m_lastCameraBlock, &cameraBlock, sizeof(cameraBlock)) == 0)
      return;
    fprintf(stderr, "cameraBlock: (%d, %d, %d)\n",
        cameraBlock.x,
        cameraBlock.y,
        cameraBlock.z);
    memcpy(&m_lastCameraBlock, &cameraBlock, sizeof(cameraBlock));

    // Iterate over all of the levels of detail we want to generate
    for (int lod = m_minimumLod; lod >= 0; --lod) {
      fprintf(stderr, "lod: %d\n", lod);
      auto cameraNode = m_lodTree.getNode(cameraBlock, lod);
      // Iterate over the voxel blocks around the camera
      for (int z = -1; z <= 1; ++z) {
        for (int y = -1; y <= 1; ++y) {
          for (int x = -1; x <= 1; ++x) {
            LodTree::Coordinates offset;
            offset.x = x;
            offset.y = y;
            offset.z = z;
            auto node = m_lodTree.getRelativeNode(*cameraNode, offset);
            // Mark this node for terrain generation at this level of detail
            m_requestDetail(*node);
          }
        }
      }
    }
  }

  void Terrain::m_requestDetail(LodTree::Node &node) {
    // Generate terrain for this node and all of its siblings. It is necessary
    // to generate terrain for all of the siblings at once so that the meshes
    // generated will match up with meshes of lower levels of detail in larger
    // nodes.
    auto parent = node.parent();
    fprintf(stderr, "m_requsetDetail: node->lod(): %d\n", node.lod());
    for (int i = 0; i < 8; ++i) {
      auto sibling = parent->getChild(i);
      // FIXME: It doesn't feel right tampering with the node state machine
      // from outside of the node class, but for now this is acceptable.
      switch (sibling->state()) {
        case LodTree::Node::State::INITIAL:
          sibling->setState(LodTree::Node::State::REQUESTED);
          m_generateTerrain(sibling);
          break;
      }
    }
  }

  void Terrain::m_generateTerrain(std::shared_ptr<LodTree::Node> node) {
    // Create a task for generating the terrain at this node
    auto terrainTask = std::shared_ptr<GenerateTerrainTask>(
        new GenerateTerrainTask(
          node,  // node
          this  // terrain
          ));
    m_workers.dispatch(terrainTask);
  }

  void Terrain::m_handleNewMesh(
      std::shared_ptr<TerrainMesh> mesh,
      std::shared_ptr<LodTree::Node> node)
  {
    // Check if we can now add the node and its mesh to the scene
    fprintf(stderr, "Handling new mesh, block: (%d, %d, %d,), lod: %d\n",
        node->block().x,
        node->block().y,
        node->block().z,
        node->lod());
    node->setMesh(mesh, this);
  }

  void Terrain::addRecentMesh(
      std::shared_ptr<TerrainMesh> mesh,
      std::shared_ptr<LodTree::Node> node)
  {
    RecentMesh recentMesh;
    // Add this mesh to the list of recent meshes
    std::unique_lock<std::mutex> lock(m_recentMeshesMutex);
    recentMesh.mesh = mesh;
    recentMesh.node = node;
    m_recentMeshes.push(recentMesh);
  }

  Terrain::RecentMesh Terrain::m_getRecentMesh() {
    RecentMesh result;
    std::unique_lock<std::mutex> lock(m_recentMeshesMutex);
    if (m_recentMeshes.empty()) {
      // Indicate an empty queue with null pointers
      result.mesh = nullptr;
      result.node = nullptr;
      return result;
    }
    // Remove and return a recent mesh from the queue of recent meshes
    result = m_recentMeshes.front();
    m_recentMeshes.pop();
    return result;
  }
} } }
