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

#ifndef MC_SAMPLES_TERRAIN_TERRAIN_H_
#define MC_SAMPLES_TERRAIN_TERRAIN_H_

#include "../common/sceneObject.h"
#include "lodTree.h"
#include "terrainGenerator.h"

namespace mc { namespace samples {
  class Camera;
  namespace terrain {
    class TerrainMesh;
    /**
     * A voxel terrain representing the given scalar field function. The terrain
     * mesh is comprised of many smaller meshes that must be stitched together by
     * the underlying isosurface extraction algorithm.
     */
    class Terrain : public SceneObject {
      private:
        std::shared_ptr<Camera> m_camera;
        // FIXME: There are two LOD tree objects, one in Terrain and one in
        // TerrainGenerator.
        LodTree m_lodTree;
        /* std::priority_queue<std::shared_ptr<LodNode>> m_meshQueue; */

        LodTree::Coordinates m_lastBlock;

        TerrainGenerator m_terrainGenerator;
        GLuint m_cubeWireframeVertices, m_cubeWireframeIndices;

        typedef struct WireframeVertex {
          float pos[3];
          float color[3];
        } WireframeVertex;

        void m_generateCubeWireframe();
        void m_drawLodOctree(
            const glm::mat4 &modelWorld,
            const glm::mat4 &worldView,
            const glm::mat4 &projection) const;

        void m_enqueueTerrain(const glm::vec3 &cameraPos);
      public:
        static constexpr int MINIMUM_LOD = 8;

        /**
         * Construct a voxel terrain object.
         *
         * \param camera The camera viewing the terrain, the position and
         * orientation of which determine the level of detail of the terrain
         * generated.
         *
         * \todo Allow the user to pass a scalar field functor.
         */
        Terrain(std::shared_ptr<Camera> camera);

        /**
         * Implements the tick() method to update the terrain mesh LOD as the
         * main camera moves through the scene.
         */
        void tick(float dt);

        /**
         * The terrain class implements a draw method for drawing debug
         * information about the LOD octree. Note that this method does not
         * actually draw any terrain. Terrain is drawn by TerrainMesh objects
         * that are children of the Terrain object.
         */
        void draw(const glm::mat4 &modelWorld,
            const glm::mat4 &worldView, const glm::mat4 &projection,
            float alpha, bool debug) const;
    };
  }
} }

#endif
