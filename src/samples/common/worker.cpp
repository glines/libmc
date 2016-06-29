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

#include <cassert>

#include "task.h"
#include "workerPool.h"

#include "worker.h"

namespace mc { namespace samples {
  Worker::Worker(WorkerPool *pool)
    : m_pool(pool), m_task(nullptr), m_taskReady(),
    m_thread([this] { this->m_run(); }), m_join(false)
  {
  }

  Worker::~Worker()
  {
    this->join();
  }

  void Worker::run(std::shared_ptr<Task> task) {
    {
      // Set the task to be run
      std::unique_lock<std::mutex> lock(m_taskMutex);
      assert(!m_task);
      m_task = task;
    }
    // Wake up the worker thread
    m_taskReady.notify_one();
  }

  void Worker::join() {
    {
      // Lock the task mutex to set the join flag
      std::unique_lock<std::mutex> lock(m_taskMutex);
      m_join = true;
    }
    // Wake up the worker thread
    m_taskReady.notify_one();
  }

  void Worker::m_run() {
    while (true) {
      // Lock the task mutex early to avoid racing the main thread
      std::unique_lock<std::mutex> lock(m_taskMutex);
      // Add this worker to the queue of ready workers
      m_pool->addReadyWorker(this);
      // Wait for a task to be assigned to this worker
      while (!m_task && !m_join) {  // Loop to avoid spurious wakeup
        m_taskReady.wait(lock);
      }
      if (m_join)
        break;  // Join the main thread
      // Run the task
      m_task->run();
      m_task = nullptr;
    }
  }
} }
