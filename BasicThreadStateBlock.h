#pragma once
#include MEMORY_HEADER
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

struct BasicThreadStateBlock:
  std::enable_shared_from_this<BasicThreadStateBlock>
{
  ~BasicThreadStateBlock();

  // General purpose thread lock and update condition for the lock
  boost::mutex m_lock;
  boost::condition_variable m_stateCondition;

  // The current thread, if running
  boost::thread m_thisThread;
};

