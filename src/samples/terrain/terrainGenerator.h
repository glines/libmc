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

#ifndef MC_SAMPLES_TERRAIN_TERRAIN_GENERATOR_H_
#define MC_SAMPLES_TERRAIN_TERRAIN_GENERATOR_H_

#include <mcxx/scalarField.h>
#include <mutex>
#include <queue>
#include <thread>

#include "../common/workerPool.h"
#include "lodTree.h"
#include "terrainMesh.h"

namespace mc { namespace samples { namespace terrain {
  /**
   * This class encapsulates the complicated heuristics used to determine which
   * terrain meshes should be generated when and at what level of detail. The
   * Terrain class makes requests for meshes at certain levels of detail based
   * on the position and orientation of the camera, and this class determines
   * which meshes need to be generated in a thread. Once meshes have finished
   * generating, they are added to a queue to be added to the scene on the next
   * frame.
   *
   * This class also handles culling old meshes at levels of detail that are no
   * longer needed.
   */
  class TerrainGenerator {
    private:
      ScalarField m_sf;

      LodTree m_lodTree;

      WorkerPool m_workers;

      typedef struct MarkedNode {
        LodTree::Coordinates block;
        int lod;

        bool operator<(const MarkedNode &other) const {
          return this->lod < other.lod;
        }
      } MarkedNode;
      std::priority_queue<MarkedNode> m_markedNodes;
      // FIXME: Clearly we cannot send data to the GL within a thread other
      // than the main thread. Also, it is not desirable to allocate memory for
      // a mesh, copy the mcMesh to that memory, and then copy that memory to
      // the GL from within the main thread. The mesh generation thread should
      // prepare memory in a format suitable for immediate upload to the GL
      // from the main thread. This will require the cooperation of the
      // mc::samples::MeshObject class, which is not ideal.
      // mc::samples::MeshObject should define the intermediate mesh format
      // that it requires for uploading to the GL. If mcMesh can be provided in
      // a flat format suitable for the GL, that might be even better, but it
      // would expose details of the mcMesh structure.
      std::queue<std::shared_ptr<TerrainMesh>> m_recentMeshes;  // XXX
      std::mutex m_recentMeshesMutex;

      std::thread m_generatorThread;

      void m_enqueueNode(const LodTree::Node &node);

    public:
      /**
       * Constructs a terrain generator object. This starts a single terrain
       * generation thread which remains idle until requests for terrain detail
       * are received.
       *
       * \param sf The scalar field that defines the terrain with an
       * isosurface.
       */
      TerrainGenerator(const ScalarField &sf);

      /**
       * This method marks the given voxel block indicating that it should be
       * generated at at least the given level of detail. The terrain generator
       * uses this as a hint as to which terrain meshes it should generate.
       *
       * This method is asynchronous, since meshes are generated in a separate
       * thread. Furthermore, there is no guarantee that a mesh of the given
       * level of detail at the given block will be generated soon, if at all.
       * The TerrainGenerator class uses its own logic to decide what meshes
       * are to be generated, and it is likely to generate lower levels of
       * detail before committing resources to generate higher levels of
       * detail.
       */
      void requestDetail(const LodTree::Coordinates &block, int lod);

      /**
       * Method adds the given mesh to the queue of recently generated meshes.
       * This is called from the GenerateTerrainTask to pass meshes 
       *
       * Since this method is intended to be called from a separate thread, the
       * method is made thread safe.
       *
       * \param mesh Shared pointer to the terrain mesh being added.
       */
      void addRecentMesh(std::shared_ptr<TerrainMesh> mesh);

      /**
       * This method removes a pointer to a recently generated terrain mesh
       * from the recently generated queue and returns it. When there are no
       * more recently generated meshes to return, this method returns a null
       * pointer.
       *
       * \return Pointer to a recently generated terrain mesh, or a null
       * pointer if there are no more recently generated meshes.
       */
      std::shared_ptr<TerrainMesh> getRecentMesh();

      /**
       * \return A reference to the scalar field whose isosurface defines the
       * surface of the terrain.
       */
      const ScalarField &sf() const { return m_sf; }
  };
} } }

#endif
