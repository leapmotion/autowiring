// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "BasicThread.h"
#include "BasicThreadStateBlock.h"
#include <pthread.h>
#include <libproc.h>
#include <mach/thread_info.h>
#include <mach/thread_act.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include THREAD_HEADER

// Missing definitions from pthreads.h
#if !defined(PTHREAD_MIN_PRIORITY)
#define PTHREAD_MIN_PRIORITY  0
#endif
#if !defined(PTHREAD_MAX_PRIORITY)
#define PTHREAD_MAX_PRIORITY 63
#endif

using std::chrono::milliseconds;
using std::chrono::nanoseconds;

void BasicThread::SetCurrentThreadName(void) const {
  pthread_setname_np(m_name);
}

std::chrono::steady_clock::time_point BasicThread::GetCreationTime(void) {
  return std::chrono::steady_clock::time_point::min();
}

void BasicThread::GetThreadTimes(std::chrono::milliseconds& kernelTime, std::chrono::milliseconds& userTime) {
  // Obtain the thread port from the Unix pthread wrapper
  pthread_t pthread = m_state->m_thisThread.native_handle();
  thread_t threadport = pthread_mach_thread_np(pthread);

  // Now use the Mac thread type to obtain the kernel thread handle
  thread_identifier_info_data_t identifier_info;
  mach_msg_type_number_t tident_count = THREAD_IDENTIFIER_INFO_COUNT;
  thread_info(threadport, THREAD_IDENTIFIER_INFO, (thread_info_t) &identifier_info, &tident_count);

  // Finally, we can obtain the actual thread times the user wants to know about
  proc_threadinfo info;
  proc_pidinfo(getpid(), PROC_PIDTHREADINFO, identifier_info.thread_handle, &info, sizeof(info));

  // User time is in ns increments
  kernelTime = std::chrono::duration_cast<milliseconds>(nanoseconds(info.pth_system_time));
  userTime = std::chrono::duration_cast<milliseconds>(nanoseconds(info.pth_user_time));
}

void BasicThread::SetThreadPriority(const std::thread::native_handle_type& handle, ThreadPriority threadPriority, SchedulingPolicy schedPolicy) {
  struct sched_param param = { 0 };
  int policy;
  int percent = 0;

  switch (schedPolicy) {
  case SchedulingPolicy::StandardRoundRobin:
    policy = SCHED_OTHER;
    break;
  case SchedulingPolicy::RealtimeFIFO:
    policy = SCHED_FIFO;
    break;
  case SchedulingPolicy::RealtimeRoundRobin:
    policy = SCHED_RR;
    break;
  default:
    throw std::invalid_argument("Attempted to assign an unrecognized scheduling policy");
    break;
  }

  switch (threadPriority) {
  case ThreadPriority::Idle:
    percent = 0;
    break;
  case ThreadPriority::Lowest:
    percent = 1;
    break;
  case ThreadPriority::BelowNormal:
    percent = 20;
    break;
  case ThreadPriority::Normal:
  case ThreadPriority::Default:
    percent = 50;
    break;
  case ThreadPriority::AboveNormal:
    percent = 66;
    break;
  case ThreadPriority::Highest:
    percent = 83;
    break;
  case ThreadPriority::TimeCritical:
    percent = 99;
    break;
  case ThreadPriority::Multimedia:
    percent = 100;
    break;
  default:
    throw std::invalid_argument("Attempted to assign an unrecognized thread priority");
  }
  int prev_policy;
  pthread_getschedparam(handle, &prev_policy, &param);
  param.sched_priority = PTHREAD_MIN_PRIORITY + (percent*(PTHREAD_MAX_PRIORITY - PTHREAD_MIN_PRIORITY) + 50) / 100;
  pthread_setschedparam(handle, policy, &param);
}
