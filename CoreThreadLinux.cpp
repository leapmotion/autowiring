// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "BasicThread.h"
#include "BasicThreadStateBlock.h"
#include <pthread.h>
#include <sys/resource.h>

void BasicThread::SetCurrentThreadName(void) const {
  if(IS_INTERNAL_BUILD)
    pthread_setname_np(pthread_self(), m_name);
}

boost::chrono::system_clock::time_point BasicThread::GetCreationTime(void) {
  return boost::chrono::system_clock::time_point::min();
}

void BasicThread::GetThreadTimes(boost::chrono::nanoseconds& kernelTime, boost::chrono::nanoseconds& userTime) {
  rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  kernelTime = boost::chrono::seconds(usage.ru_stime.tv_sec) + boost::chrono::microseconds(usage.ru_stime.tv_usec);
  userTime = boost::chrono::seconds(usage.ru_utime.tv_sec) + boost::chrono::microseconds(usage.ru_utime.tv_usec);
}