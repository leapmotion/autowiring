#include "stdafx.h"
#include "CoreThread.h"
#include "CoreContext.h"
#include <boost/thread.hpp>

using namespace boost;
using namespace std;

CoreThread::CoreThread(const char* pName):
  ContextMember(pName),
  m_stop(true)
{
}

CoreThread::~CoreThread(void) {
}

bool CoreThread::ShouldStop(void) const {
  return m_stop || (m_context && m_context->ShouldStop());
}

void CoreThread::ThreadSleep(long millisecond) {
  this_thread::sleep(boost::posix_time::milliseconds(millisecond));
}

