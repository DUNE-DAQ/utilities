/**
 * @file WorkerThread.cpp WorkerThread class definitions
 *
 * WorkerThread defines a std::thread which runs the do_work()
 * function as well as methods to start and stop that thread.
 * This file is intended to help reduce code duplication for the common
 * task of starting and stopping threads. As in artdaq, std::thread may
 * be replaced by boost::thread to allow setting the stack size at a
 * later date if that functionality is found to be necessary.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "utilities/WorkerThread.hpp"

dunedaq::utilities::WorkerThread::WorkerThread(std::function<void(std::atomic<bool>&)> do_work)
  : m_thread_running(false)
  , m_working_thread(nullptr)
  , m_do_work(do_work)
{
}

void
dunedaq::utilities::WorkerThread::start_working_thread(const std::string& name)
{
  if (thread_running()) {
    throw ThreadingIssue(ERS_HERE,
                         "Attempted to start working thread "
                         "when it is already running!");
  }
  m_thread_running = true;
  m_working_thread.reset(new std::thread([&] { m_do_work(std::ref(m_thread_running)); }));
  auto handle = m_working_thread->native_handle();
  auto rc = pthread_setname_np(handle, name.c_str());
  if (rc != 0) {
    std::ostringstream s;
    s << "The name " << name << " provided for the thread is too long.";
    ers::warning(ThreadingIssue(ERS_HERE, s.str()));
  }
}

void
dunedaq::utilities::WorkerThread::stop_working_thread()
{
  if (!thread_running()) {
    throw ThreadingIssue(ERS_HERE,
                         "Attempted to stop working thread "
                         "when it is not running!");
  }
  m_thread_running = false;

  if (m_working_thread->joinable()) {
    try {
      m_working_thread->join();
    } catch (std::system_error const& e) {
      throw ThreadingIssue(ERS_HERE, std::string("Error while joining thread, ") + e.what());
    }
  } else {
    throw ThreadingIssue(ERS_HERE, "Thread not in joinable state during working thread stop!");
  }
}
