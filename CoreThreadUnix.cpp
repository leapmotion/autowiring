// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "BasicThread.h"
#include "BasicThreadStateBlock.h"
#include <pthread.h>
#include <sys/resource.h>

void BasicThread::SetCurrentThreadName(void) const {
  if(IS_INTERNAL_BUILD)
#if __APPLE__
    pthread_setname_np(m_name);
#else
    pthread_setname_np(pthread_self(), m_name);
#endif
}

std::chrono::steady_clock::time_point BasicThread::GetCreationTime(void) {
  return std::chrono::steady_clock::time_point::min();
}

void BasicThread::GetThreadTimes(std::chrono::nanoseconds& kernelTime, std::chrono::nanoseconds& userTime) {
  rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  kernelTime = std::chrono::seconds(usage.ru_stime.tv_sec) + std::chrono::microseconds(usage.ru_stime.tv_usec);
  userTime = std::chrono::seconds(usage.ru_utime.tv_sec) + std::chrono::microseconds(usage.ru_utime.tv_usec);
}
