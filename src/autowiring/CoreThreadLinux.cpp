// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "BasicThread.h"
#include "BasicThreadStateBlock.h"
#include <pthread.h>
#include <sys/resource.h>

using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::microseconds;

void BasicThread::SetCurrentThreadName(void) const {
  pthread_setname_np(pthread_self(), m_name);
}

std::chrono::steady_clock::time_point BasicThread::GetCreationTime(void) {
  return std::chrono::steady_clock::time_point::min();
}

void BasicThread::GetThreadTimes(std::chrono::milliseconds& kernelTime, std::chrono::milliseconds& userTime) {
  rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  kernelTime = std::chrono::duration_cast<milliseconds>(seconds(usage.ru_stime.tv_sec) + microseconds(usage.ru_stime.tv_usec));
  userTime = std::chrono::duration_cast<milliseconds>(seconds(usage.ru_utime.tv_sec) + microseconds(usage.ru_utime.tv_usec));
}
