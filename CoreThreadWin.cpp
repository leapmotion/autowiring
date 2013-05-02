#include "stdafx.h"
#include "CoreThread.h"
#include <Windows.h>

typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // must be 0x1000
   LPCSTR szName; // pointer to name (in user addr space)
   DWORD dwThreadID; // thread ID (-1=caller thread)
   DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;

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
    RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info);
  }
  __except(EXCEPTION_CONTINUE_EXECUTION)
  {
  }
}

void CoreThread::SetCurrentThreadName(void) const {
  ::SetThreadName(
    GetCurrentThreadId(),
    m_name
  );
}