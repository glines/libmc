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

#include "terrainGenerator.h"
#include "terrainMesh.h"

#include "generateTerrainTask.h"

namespace mc { namespace samples { namespace terrain {
  GenerateTerrainTask::GenerateTerrainTask(
      const LodTree::Coordinates block, int lod,
      TerrainGenerator *generator)
    : m_generator(generator), m_block(block), m_lod(lod)
  {
  }

  void GenerateTerrainTask::run() {
    // TODO: Obtain a pointer to the sample field from the terrain generator
    auto sf = m_generator->sf();
    // TODO: Build a terrain mesh
    // FIXME: The MeshObject class most certainly makes some GL calls here
    auto mesh = std::shared_ptr<TerrainMesh>(
        new TerrainMesh(sf, m_block, m_lod));
    fprintf(stderr, "Generating mesh at block: (%d, %d, %d), lod: %d\n",
        m_block.x,
        m_block.y,
        m_block.z,
        m_lod);
    // TODO: Notify the terrain generator that we have a mesh
    m_generator->addRecentMesh(mesh);
  }
} } }
