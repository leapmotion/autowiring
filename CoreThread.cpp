#include "stdafx.h"
#include "CoreThread.h"
#include "CoreContext.h"
#include <boost/thread.hpp>

bool CoreThread::ShouldStop(void) const {
  shared_ptr<CoreContext> context = ContextMember::GetContext();
  return m_stop || context || context->ShouldStop();
}

void CoreThread::ThreadSleep(long millisecond) {
  boost::this_thread::sleep(boost::posix_time::milliseconds(millisecond));
}

