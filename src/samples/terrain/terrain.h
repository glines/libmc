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

#include <GL/glew.h>
#include <mcxx/scalarField.h>
#include <mutex>
#include <queue>

#include "../common/sceneObject.h"
#include "../common/workerPool.h"
#include "lodTree.h"

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
        typedef struct WireframeVertex {
          float pos[3];
          float color[3];
        } WireframeVertex;

        ScalarField m_sf;

        LodTree m_lodTree;
        int m_minimumLod;

        typedef struct RecentMesh {
          std::shared_ptr<TerrainMesh> mesh;
          std::shared_ptr<LodTree::Node> node;
        } RecentMesh;
        std::queue<RecentMesh> m_recentMeshes;
        std::mutex m_recentMeshesMutex;

        WorkerPool m_workers;

        std::shared_ptr<Camera> m_camera;
        LodTree::Coordinates m_lastCameraBlock;

        LodTree::Coordinates m_lastBlock;

        GLuint m_cubeWireframeVertices, m_cubeWireframeIndices;

        void m_generateCubeWireframe();
        void m_drawLodOctree(
            const glm::mat4 &modelWorld,
            const glm::mat4 &worldView,
            const glm::mat4 &projection) const;

        /**
         * Updates the terrain generation priorities based on the state of the
         * given camera. This method is called frequently so that the terrain
         * generator can update the terrain as the position, orientation, and
         * projection of the camera changes.
         */
        void m_updateCamera();

        void m_requestDetail(LodTree::Node &node);
        void m_generateTerrain(std::shared_ptr<LodTree::Node> node);

        void m_handleNewMesh(
            std::shared_ptr<TerrainMesh> mesh,
            std::shared_ptr<LodTree::Node> node);

        void m_popNode(std::shared_ptr<LodTree::Node> node);

        /**
         * This method removes a recently generated terrain mesh from the
         * recently generated queue and returns it. When there are no more
         * recently generated meshes to return, this method returns a struct
         * filled with null pointers.
         *
         * Since the terrain meshes are added from a thread separate from the
         * main thread, this method is made thread safe.
         *
         * \return Struct containing shared pointers to a recently generated
         * mesh and its corresponding node, or struct filled with null pointers
         * if there are no more recently generated meshes.
         */
        RecentMesh m_getRecentMesh();
      public:
        static constexpr int MINIMUM_LOD = 8;

        /**
         * Construct a voxel terrain object.
         *
         * This class encapsulates the complicated heuristics used to determine
         * which terrain meshes should be generated when and at what level of
         * detail. The class generates terrain meshs meshes at different levels
         * of detail based on the position and orientation of the camera. These
         * meshes are generated in separate threads, and once the meshes have
         * finished generating they are added to a queue to be added to the
         * scene on the next frame.
         *
         * Additionally, old meshes at levels of detail that are no longer
         * needed are periodically culled from the scene to reduce the number
         * of polygons and draw calls.
         *
         * \param camera The camera viewing the terrain, the position and
         * orientation of which determine the level of detail of the terrain
         * generated.
         * \param minimumLod The lowest level of detail that this terrain
         * generator will render. This value determines how large the lowest
         * resolution meshes will be.
         *
         * \todo Allow the user to pass a scalar field functor.
         */
        Terrain(
            std::shared_ptr<Camera> camera,
            int minimumLod = 8);

        /**
         * \return A reference to the scalar field whose isosurface defines the
         * surface of the terrain.
         */
        const ScalarField &sf() const { return m_sf; }

        /**
         * This method returns the lowest level of detail that this terrain
         * object will generate. This value ultimately decides how far up the
         * octree meshes will be generated, and how far to the horizon the
         * terrain will reach.
         * \return The lowest level of detail that this terrain object will
         * generate.
         */
        int minimumLod() const { return m_minimumLod; }

        /**
         * Method adds the given mesh to the queue of recently generated meshes.
         * This is called from the GenerateTerrainTask to pass meshes 
         *
         * Since this method is intended to be called from a separate thread,
         * the method is made thread safe.
         *
         * \param mesh Shared pointer to the terrain mesh being added.
         * \param block The coordinates of the voxel block that this mesh
         * occupies.
         * \param lod The level of detail of this mesh.
         */
        void addRecentMesh(
            std::shared_ptr<TerrainMesh> mesh,
            std::shared_ptr<LodTree::Node> node);

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
            float alpha, bool debug);
    };
  }
} }

#endif
