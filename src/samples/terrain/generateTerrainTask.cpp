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

#include "terrain.h"
#include "terrainMesh.h"

#include "generateTerrainTask.h"

namespace mc { namespace samples { namespace terrain {
  GenerateTerrainTask::GenerateTerrainTask(
      std::shared_ptr<LodTree::Node> node,
      Terrain *terrain)
    : m_terrain(terrain), m_node(node)
  {
  }

  void GenerateTerrainTask::run() {
    // Generate terrain using the scalar field for this terrain object
    auto sf = m_terrain->sf();
    auto mesh = std::shared_ptr<TerrainMesh>(
        new TerrainMesh(sf, m_node->block(), m_node->lod()));
    fprintf(stderr, "Generating mesh at block: (%d, %d, %d), lod: %d\n",
        m_node->block().x,
        m_node->block().y,
        m_node->block().z,
        m_node->lod());
    // Notify the terrain object that we have a mesh
    m_terrain->addRecentMesh(mesh, m_node);
  }
} } }
