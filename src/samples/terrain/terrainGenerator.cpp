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

#include "generateTerrainTask.h"

#include "terrainGenerator.h"

namespace mc { namespace samples { namespace terrain {
  TerrainGenerator::TerrainGenerator(const ScalarField &sf)
    : m_sf(sf), m_workers(std::thread::hardware_concurrency())
  {
  }

  void TerrainGenerator::m_enqueueNode(const LodTree::Node &node) {
    // TODO: We need to know the node block and the level of detail
    // TODO: Create a task for generating the terrain at this node
    auto terrainTask = std::shared_ptr<GenerateTerrainTask>(
        new GenerateTerrainTask(
          node.block(),  // block
          node.lod(),  // lod
          this  // generator
          ));
    m_workers.dispatch(terrainTask);
  }

  void TerrainGenerator::requestDetail(
      const LodTree::Coordinates &block, int lod)
  {
    // Traverse the LOD tree and mark all of the levels of detail up to this
    // one for generation
    auto node = m_lodTree.getNode(block, lod).get();
    bool done = false;
    while (node && !done) {
      switch (node->status()) {
        case LodTree::Node::Status::NOT_GENERATED:
          node->setStatus(LodTree::Node::Status::MARKED);
          m_enqueueNode(*node);
          break;
        default:
          // The mesh for this node has already been considerd, so all of its
          // parent nodes at lower levels of detail must have already been
          // considered.
          done = true;
          break;
      }
      node = node->parent();
    }
  }

  void TerrainGenerator::addRecentMesh(std::shared_ptr<TerrainMesh> mesh) {
    // Add this mesh to the list of recent meshes
    std::unique_lock<std::mutex> lock(m_recentMeshesMutex);
    m_recentMeshes.push(mesh);
  }

  std::shared_ptr<TerrainMesh> TerrainGenerator::getRecentMesh() {
    std::unique_lock<std::mutex> lock(m_recentMeshesMutex);
    if (m_recentMeshes.empty())
      return nullptr;
    // Remove and return a recent mesh from the queue of recent meshes
    auto result = m_recentMeshes.front();
    m_recentMeshes.pop();
    return result;
  }
} } }
