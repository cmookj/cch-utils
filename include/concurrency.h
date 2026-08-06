//
//  Concurrency.hpp
//
//  A simple threads manager.
//  Changmook Chun (c) 2024.  All rights reserved.
//

#ifndef cch_concurrency_hpp
#define cch_concurrency_hpp

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace cch::concurrency {

// Example usage
// =============
//
// 20 tasks, 10 threads (in M1 Max)
//
// 1. Enqueue jobs
// 2. Start threads
// 3. Wait while the thread pool is busy
// 4. Finally, call stop() to finish remaining threads
//
// using namespace cch::concurrency;
//
// int
// main (int argc, char* argv[]) {
//   thread_pool tp;
//
//   for (int id = 0; id < 20; ++id) {
//     tp.queue_job ([id] () {
//       for (int i = 0; i < 5; ++i) {
//         std::stringstream strm;
//         strm << "Thread [" << id << "]: " << i << '\n';
//         std::cout << strm.str();
//         std::this_thread::sleep_for (std::chrono::seconds (1));
//       }
//     });
//   }
//
//   tp.start();
//
//   while (tp.busy())
//     ;
//
//   tp.stop();
//
//   return 0;
// }
//

class thread_pool {
public:
  thread_pool ()  = default;
  ~thread_pool () = default;

  void
  start () {
    // Once threads are created according to the hardware capability,
    // it's better not to create new ones or destroy old ones (by joining).
    // There will be performance penalty, and it might even make the application
    // go slower than the serial version.
    // Thus, we keep a pool of threads that can be used at any time, if they
    // aren't already running a job.
    //
    // Each thread runs its own infinite loop, constantly waiting for new tasks
    // to grab and run.
    const uint32_t count_threads = std::thread::hardware_concurrency();
    for (uint32_t i = 0; i < count_threads; ++i) {
      _threads.emplace_back (std::thread (&thread_pool::thread_loop, this));
    }
  }

  // Add a new job to the pool.  Use a lock to prevent data race.
  // To use this function:
  //   thread_pool -> queue_job([] { /* ... */ });
  void
  queue_job (const std::function<void()>& job) {
    {
      std::unique_lock<std::mutex> lock (_queue_mutex);
      _jobs.push (job);
    }
    {
      std::unique_lock<std::mutex> lock (_count_jobs_mutex);
      _count_jobs++;
    }
    _mutex_condition.notify_one();
  }

  // Stops the pool.
  void
  stop () {
    {
      std::unique_lock<std::mutex> lock (_queue_mutex);
      _should_terminate = true;
    }
    _mutex_condition.notify_all();
    for (std::thread& active_thread : _threads) {
      active_thread.join();
    }
    _threads.clear();
  }

  // This function can be called within a while loop, e.g., the main thread can
  // wait the thread_pool to complete all the tasks before deleting the
  // thread_pool object.
  bool
  busy () {
    bool pool_busy;
    {
      std::unique_lock<std::mutex> lock (_queue_mutex);
      pool_busy = !_jobs.empty();
    }
    return pool_busy;
  }

  int
  count_jobs () {
    return _count_jobs;
  }

private:
  // The infinite loop function.  This waits for the task queue to open up.
  void
  thread_loop () {
    while (true) {
      std::function<void()> job;
      {
        std::unique_lock<std::mutex> lock (_queue_mutex);
        _mutex_condition.wait (lock, [this] { return !_jobs.empty() || _should_terminate; });
        if (_should_terminate) {
          return;
        }
        job = _jobs.front();
        _jobs.pop();
      }
      // Execute the job and decrease the number of jobs when finished.
      job();
      {
        std::unique_lock<std::mutex> lock (_count_jobs_mutex);
        _count_jobs--;
      }
    }
  }

  // Tells threads to stop looking for jobs
  bool _should_terminate = false;

  // The number of jobs
  int        _count_jobs = 0;
  std::mutex _count_jobs_mutex;

  // Prevents data races to the job queue & running threads
  std::mutex _queue_mutex;

  // Allows threads to wait on new jobs or termination
  std::condition_variable _mutex_condition;

  std::vector<std::thread> _threads;

  std::queue<std::function<void()>> _jobs;
};

}  // namespace cch::concurrency

#endif
