// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "BasicThread.h"
#include "BasicThreadStateBlock.h"
#include <pthread.h>
#include <sys/resource.h>

using boost::chrono::seconds;
using boost::chrono::milliseconds;
using boost::chrono::microseconds;

void BasicThread::SetCurrentThreadName(void) const {
  if(IS_INTERNAL_BUILD)
    pthread_setname_np(pthread_self(), m_name);
}

boost::chrono::system_clock::time_point BasicThread::GetCreationTime(void) {
  return boost::chrono::system_clock::time_point::min();
}

void BasicThread::GetThreadTimes(boost::chrono::milliseconds& kernelTime, boost::chrono::milliseconds& userTime) {
  rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  kernelTime = boost::chrono::duration_cast<milliseconds>(seconds(usage.ru_stime.tv_sec) + microseconds(usage.ru_stime.tv_usec));
  userTime = boost::chrono::duration_cast<milliseconds>(seconds(usage.ru_utime.tv_sec) + microseconds(usage.ru_utime.tv_usec));
}
