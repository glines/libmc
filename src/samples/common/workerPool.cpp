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

#include "task.h"
#include "worker.h"

#include "workerPool.h"

namespace mc { namespace samples {
  bool WorkerPool::m_compareTasks(
      std::shared_ptr<Task> left, 
      std::shared_ptr<Task> right)
  {
    return left->priority() < right->priority();
  }

  WorkerPool::WorkerPool(int numWorkers)
    : m_readyWorkers(), m_ready(), m_readyMutex(),
    m_tasks(&m_compareTasks),
    m_dispatchThread([this] { this->m_dispatchLoop(); })
  {
    for (int i = 0; i < numWorkers; ++i)
      m_workers.push_back(std::shared_ptr<Worker>(new Worker(this)));
  }

  WorkerPool::~WorkerPool()
  {
    // TODO: Join all of the worker threads
  }

  // FIXME: Why do I use unique_lock and not lock_guard or shared_lock?
  void WorkerPool::dispatch(std::shared_ptr<Task> task) {
    // Add an available task and signal the task dispatch thread
    this->addAvailableTask(task);
  }

  void WorkerPool::m_dispatchLoop() {
    // Loop for the lifetime of the class to dispatch tasks 
    while (1) {
      // Wait for a ready worker
      std::unique_lock<std::mutex> readyLock(m_readyMutex);
      while (m_readyWorkers.size() <= 0) {  // Loop to avoid spurious wakeup
        m_ready.wait(readyLock);
      }
      // NOTE: We hold the readyLock for a long time here, but it doesn't
      // matter as long as we have at least one ready worker
      // Wait for an available task
      std::unique_lock<std::mutex> taskLock(m_taskMutex);
      while (m_tasks.size() <= 0) {  // Loop to avoid spurious wakeup
        m_taskAvailable.wait(taskLock);
      }
      m_readyWorkers.back()->run(m_tasks.top());
      fprintf(stderr, "m_tasks.top()->priority(): %d\n",
          m_tasks.top()->priority());
      m_readyWorkers.pop_back();
      m_tasks.pop();
    }
  }

  void WorkerPool::addReadyWorker(Worker *worker) {
    {
      // Add this worker to the list of ready workers
      std::unique_lock<std::mutex> lock(m_readyMutex);
      m_readyWorkers.push_back(worker);
    }
    // Wake up the dispatch thread
    m_ready.notify_one();
  }

  void WorkerPool::addAvailableTask(std::shared_ptr<Task> task) {
    {
      // Add this task to the queue of available tasks
      std::unique_lock<std::mutex> lock(m_taskMutex);
      m_tasks.push(task);
    }
    // Wake up the dispatch thread
    m_taskAvailable.notify_one();
  }
} }
