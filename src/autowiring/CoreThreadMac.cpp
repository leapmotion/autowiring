// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
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
#include THREAD_HEADER

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
