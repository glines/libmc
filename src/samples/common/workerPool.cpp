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

#include "worker.h"

#include "workerPool.h"

namespace mc { namespace samples {
  WorkerPool::WorkerPool(int numWorkers)
    : m_readyWorkers(), m_ready(), m_readyMutex()
  {
    for (int i = 0; i < numWorkers; ++i)
      m_workers.push_back(std::shared_ptr<Worker>(new Worker(this)));
  }

  WorkerPool::~WorkerPool()
  {
    // TODO: Join all of the worker threads
  }

  void WorkerPool::dispatch(std::shared_ptr<Task> task) {
    // Wait for a worker thread to become available
    std::unique_lock<std::mutex> lock(m_readyMutex);
    while (m_readyWorkers.size() <= 0) {  // Loop to avoid spurious wakeup
      m_ready.wait(lock);
    }
    // Run the task on an available worker
    m_readyWorkers.back()->run(task);
    m_readyWorkers.pop_back();
  }

  void WorkerPool::addReadyWorker(Worker *worker) {
    {
      // Add this worker to the list of ready workers
      std::unique_lock<std::mutex> lock(m_readyMutex);
      m_readyWorkers.push_back(worker);
    }
    // Wake up the main thread
    m_ready.notify_one();
  }
} }
