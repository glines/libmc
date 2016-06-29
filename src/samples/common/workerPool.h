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

#ifndef MC_SAMPLES_COMMON_WORKER_POOL_H_
#define MC_SAMPLES_COMMON_WORKER_POOL_H_

#include <condition_variable>
#include <memory>
#include <vector>

namespace mc { namespace samples {
  class Task;
  class Worker;
  class WorkerPool {
    friend Worker;
    private:
      std::vector<std::shared_ptr<Worker>> m_workers;
      std::vector<Worker *> m_readyWorkers;
      std::condition_variable m_ready;
      std::mutex m_readyMutex;
    public:
      WorkerPool(int numWorkers);
      ~WorkerPool();

      void dispatch(std::shared_ptr<Task> task);
    private:
      void addReadyWorker(Worker *worker);
  };
} }

#endif
