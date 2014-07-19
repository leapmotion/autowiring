// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
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
#include <iostream>
#include <thread>

void BasicThread::SetCurrentThreadName(void) const {
  if(IS_INTERNAL_BUILD)
    pthread_setname_np(m_name);
}

boost::chrono::system_clock::time_point BasicThread::GetCreationTime(void) {
  return boost::chrono::system_clock::time_point::min();
}

void BasicThread::GetThreadTimes(boost::chrono::milliseconds& kernelTime, boost::chrono::milliseconds& userTime) {
  // Obtain the thread port from the Unix pthread wrapper
  pthread_t pthread = m_state->m_thisThread.native_handle();
  thread_t threadport = pthread_mach_thread_np(pthread);

  // Now use the Mac thread type to obtain the kernel thread handle
  thread_identifier_info_data_t identifier_info;
  mach_msg_type_number_t tident_count = THREAD_IDENTIFIER_INFO_COUNT;
  thread_info(threadport, THREAD_IDENTIFIER_INFO, (thread_info_t) &identifier_info, &tident_count);

  // Finally, we can obtain the actual thread times the user wants to know about
  proc_threadinfo info = {};
  proc_pidinfo(getpid(), PROC_PIDTHREADINFO, identifier_info.thread_handle, &info, sizeof(info));

  // User time is in 100-ns increments
  kernelTime = boost::chrono::milliseconds(info.pth_system_time/1000000);
  userTime = boost::chrono::milliseconds(info.pth_user_time/1000000);
}