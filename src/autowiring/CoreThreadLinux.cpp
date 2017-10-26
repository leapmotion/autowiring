// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "BasicThread.h"
#include "BasicThreadStateBlock.h"
#include <pthread.h>
#include <sys/resource.h>
#include <pthread.h>
#include <sched.h>

using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::microseconds;

void BasicThread::SetCurrentThreadName(void) const {
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
  int policy = SCHED_OTHER;
  int percent = 0;
  int min_priority;

  switch (threadPriority) {
  case ThreadPriority::Idle:
//Android kernel(3.10.x) has not implemented SCHED_IDLE yet.
#ifndef __ANDROID__
    policy = SCHED_IDLE;
#endif
    percent = 0;
    break;
  case ThreadPriority::Lowest:
    percent = 1;
    break;
  case ThreadPriority::BelowNormal:
    percent = 20;
    break;
  case ThreadPriority::Default:
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
  case ThreadPriority::Multimedia:
    percent = 100;
    break;
  default:
    throw std::invalid_argument("Attempted to assign an unrecognized thread priority");
  }
  min_priority = sched_get_priority_min(policy);
  pthread_getschedparam(m_state->m_thisThread.native_handle(), &policy, &param);
  param.sched_priority = min_priority + (percent * (sched_get_priority_max(policy) - min_priority) + 50) / 100;
  pthread_setschedparam(m_state->m_thisThread.native_handle(), policy, &param);
  m_priority = threadPriority;
}
