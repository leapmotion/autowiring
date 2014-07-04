// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "BasicThread.h"
#include <pthread.h>

void BasicThread::SetCurrentThreadName(void) const {
  if(IS_INTERNAL_BUILD)
#if __APPLE__
    pthread_setname_np(m_name);
#else
    pthread_setname_np(pthread_self(), m_name);
#endif
}

boost::chrono::system_clock::time_point BasicThread::GetCreationTime(void) {
  return boost::chrono::system_clock::time_point::min();
}

void BasicThread::GetThreadTimes(boost::chrono::nanoseconds& kernelTime, boost::chrono::nanoseconds& userTime) {
}