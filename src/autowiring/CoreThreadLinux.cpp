// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "BasicThread.h"
#include "BasicThreadStateBlock.h"
#include <pthread.h>
#include <sys/resource.h>
#include <pthread.h>

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

void BasicThread::SetThreadPriority(ThreadPriority threadPriority) {
  struct sched_param param = { 0 };
  int policy = SCHED_RR;
  int percent = 0;

  switch (threadPriority) {
  case ThreadPriority::Idle:
    policy = SCHED_IDLE;
    percent = 0;
    break;
  case ThreadPriority::Lowest:
    percent = 1;
    break;
  case ThreadPriority::BelowNormal:
    percent = 20;
    break;
  case ThreadPriority::Normal:
    percent = 50;
    break;
  case ThreadPriority::AboveNormal:
    percent = 66;
    break;
  case ThreadPriority::Highest:
    percent = 83;
    break;
  case ThreadPriority::TimeCritical:
    percent = 100;
    break;
  default:
    throw std::runtime_error("Attempted to assign an unrecognized thread priority");
  }
  param.sched_priority = PTHREAD_MIN_PRIORITY + (percent*(PTHREAD_MAX_PRIORITY - PTHREAD_MIN_PRIORITY) + 50) / 100;

  pthread_setschedparam(m_thisThread.native_handle(), policy, &param);
}
