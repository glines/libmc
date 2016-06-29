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

#include <mcxx/isosurfaceBuilder.h>
#include <mcxx/vector.h>

#include "terrainMesh.h"

namespace mc { namespace samples { namespace terrain {
  TerrainMesh::TerrainMesh(const ScalarField &sf,
      int lod, int x, int y, int z)
    : MeshObject(
        glm::vec3(
          (float)x * VOXEL_DELTA * (float)BLOCK_SIZE,
          (float)y * VOXEL_DELTA * (float)BLOCK_SIZE,
          (float)z * VOXEL_DELTA * (float)BLOCK_SIZE),  // position
        glm::quat()  // orientation
        )
  {
    // Generate the terrain mesh by extracting the isosurface for the
    // given scalar field
    // TODO: Implement support for more than one level of detail in the terrain
    // mesh.
    IsosurfaceBuilder ib;
    auto mesh = ib.buildIsosurface(
        sf,  // scalar field
        MC_ORIGINAL_MARCHING_CUBES,  // algorithm
        BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE,  // resolution
        Vec3(
          this->position().x,
          this->position().y,
          this->position().z),  // min
        Vec3(
          this->position().x + (float)BLOCK_SIZE * VOXEL_DELTA * (1 << lod),
          this->position().y + (float)BLOCK_SIZE * VOXEL_DELTA * (1 << lod),
          this->position().z + (float)BLOCK_SIZE * VOXEL_DELTA * (1 << lod)) // max
        );
    m_empty = mesh->numVertices() == 0;
    if (!m_empty) {
      this->setMesh(*mesh);
    }
  }
} } }
