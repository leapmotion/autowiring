// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "CoreThread.h"
#include <Windows.h>

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // must be 0x1000
   LPCSTR szName; // pointer to name (in user addr space)
   DWORD dwThreadID; // thread ID (-1=caller thread)
   DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
{
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = szThreadName;
  info.dwThreadID = dwThreadID;
  info.dwFlags = 0;

  __try
  {
    // Magic exception which informs the OS of this thread's name
    RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*)&info);
  }
  __except(EXCEPTION_CONTINUE_EXECUTION)
  {
  }
}

void CoreThread::SetCurrentThreadName(void) const {
#if IS_INTERNAL_BUILD == 1
  ::SetThreadName(
    GetCurrentThreadId(),
    m_name
  );
#endif
}
