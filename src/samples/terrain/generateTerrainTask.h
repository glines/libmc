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

#ifndef MC_SAMPLES_TERRAIN_GENERATE_TERRAIN_TASK_H_
#define MC_SAMPLES_TERRAIN_GENERATE_TERRAIN_TASK_H_

#include "../common/task.h"
#include "lodTree.h"

namespace mc { namespace samples { namespace terrain {
  class TerrainGenerator;
  /**
   * Class that represents the task of generating a single terrain mesh object
   * for a given terrain mesh generator. Terrain generation tasks can be
   * submitted to a WorkerPool for execution in a thread by a Worker object.
   * Since terrain generation is an expensive task, running it in a thread
   * greatly improves responsiveness of the user interface.
   *
   * This class communicates the status of the generated terrain mesh with the
   * TerrainGenerator class using thread-safe methods.
   */
  class GenerateTerrainTask : public Task {
    private:
      TerrainGenerator *m_generator;
      LodTree::Coordinates m_block;
      int m_lod;
    public:
      GenerateTerrainTask(
          const LodTree::Coordinates block, int lod,
          TerrainGenerator *generator);

      void run();

      int priority() const { return m_lod; }
  };
} } }

#endif
