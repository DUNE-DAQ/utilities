/**
 * @file ReusableThread.cpp Reusable thread wrapper
 * The same thread instance can be used with different tasks to be executed
 * Inspired by:
 * https://codereview.stackexchange.com/questions/134214/reuseable-c11-thread
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/ReusableThread.hpp"

dunedaq::utilities::ReusableThread::ReusableThread(int threadid)
  : m_thread_id(threadid)
  , m_task_executed(true)
  , m_task_assigned(false)
  , m_thread_quit(false)
  , m_worker_done(false)
  , m_thread(&ReusableThread::thread_worker, this)
{
}

dunedaq::utilities::ReusableThread::~ReusableThread()
{
  while (m_task_assigned) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  m_thread_quit = true;
  while (!m_worker_done) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    m_cv.notify_all();
  }
  m_thread.join();
}

void
dunedaq::utilities::ReusableThread::set_name(const std::string& name, int tid)
{
  set_thread_id(tid);
  char tname[16];
  snprintf(tname, 16, "%s-%d", name.c_str(), tid); // NOLINT
  auto handle = m_thread.native_handle();
  pthread_setname_np(handle, tname);
}

void
dunedaq::utilities::ReusableThread::thread_worker()
{
  std::unique_lock<std::mutex> lock(m_mtx);

  while (!m_thread_quit) {
    if (!m_task_executed && m_task_assigned) {
      m_task();
      m_task_executed = true;
      m_task_assigned = false;
    } else {
      m_cv.wait(lock);
    }
  }

  m_worker_done = true;
}
