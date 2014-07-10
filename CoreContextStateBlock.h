#pragma once
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

struct CoreContextStateBlock
{
public:
  CoreContextStateBlock(void);
  ~CoreContextStateBlock(void);

  // General purpose lock for this class
  std::mutex m_lock;

  // Condition, signalled when context state has been changed
  std::condition_variable m_stateChanged;
};

