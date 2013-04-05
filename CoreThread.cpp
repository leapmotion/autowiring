#include "stdafx.h"
#include "CoreThread.h"
#include "CoreContext.h"
#include <boost/thread.hpp>

bool CoreThread::ShouldStop(void) const {
  return m_stop || (m_context && m_context->ShouldStop());
}

void CoreThread::ThreadSleep(long millisecond) {
  boost::this_thread::sleep(boost::posix_time::milliseconds(millisecond));
}

