#ifndef _SIMPLE_THREADED_H
#define _SIMPLE_THREADED_H
#include "Autowiring/CoreThread.h"

class SimpleThreaded:
  public CoreThread
{
public:
  SimpleThreaded(void) {
    Ready();
  }

  // Signal this condition to get the thread to exit
  boost::mutex m_condLock;
  boost::condition_variable m_cond;

  void Run(void) {
    boost::unique_lock<boost::mutex> lk(m_condLock);
    m_cond.wait(lk, [this] () {return ShouldStop();});
  }
};

#endif
