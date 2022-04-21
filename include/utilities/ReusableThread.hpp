/**
 * @file ReusableThread.hpp Reusable thread wrapper
 * The same thread instance can be used with different tasks to be executed
 * Inspired by:
 * https://codereview.stackexchange.com/questions/134214/reuseable-c11-thread
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef UTILITIES_INCLUDE_UTILITIES_REUSABLETHREAD_HPP_
#define UTILITIES_INCLUDE_UTILITIES_REUSABLETHREAD_HPP_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

namespace dunedaq {
namespace utilities {

class ReusableThread
{
public:
  explicit ReusableThread(int threadid);

  ~ReusableThread();

  ReusableThread(const ReusableThread&) = delete;            ///< ReusableThread is not copy-constructible
  ReusableThread& operator=(const ReusableThread&) = delete; ///< ReusableThread is not copy-assginable
  ReusableThread(ReusableThread&&) = delete;                 ///< ReusableThread is not move-constructible
  ReusableThread& operator=(ReusableThread&&) = delete;      ///< ReusableThread is not move-assignable

  // Set thread ID
  void set_thread_id(int tid) { m_thread_id = tid; }

  // Get thread ID
  int get_thread_id() const { return m_thread_id; }

  // Set name for pthread handle
  void set_name(const std::string& name, int tid);

  // Check for completed task execution
  bool get_readiness() const { return m_task_executed; }

  // Set task to be executed
  template<typename Function, typename... Args>
  bool set_work(Function&& f, Args&&... args)
  {
    if (!m_task_assigned && m_task_executed.exchange(false)) {
      m_task = std::bind(f, args...);
      m_task_assigned = true;
      m_cv.notify_all();
      return true;
    }
    return false;
  }

private:
  // Internals
  int m_thread_id;
  std::atomic<bool> m_task_executed;
  std::atomic<bool> m_task_assigned;
  std::atomic<bool> m_thread_quit;
  std::atomic<bool> m_worker_done;
  std::function<void()> m_task;

  // Locks
  std::mutex m_mtx;
  std::condition_variable m_cv;
  std::thread m_thread;

  // Actual worker thread
  void thread_worker();
};

} // namespace utilities
} // namespace dunedaq

#endif // UTILITIES_INCLUDE_UTILITIES_REUSABLETHREAD_HPP_
