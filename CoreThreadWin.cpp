#include "stdafx.h"
#include "BasicThread.h"
#include "BasicThreadStateBlock.h"
#include CHRONO_HEADER
#include <Windows.h>
#include <Avrt.h>

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

void BasicThread::SetCurrentThreadName(void) const {
  if(IS_INTERNAL_BUILD)
    ::SetThreadName(m_state->m_thisThread.get_thread_info()->id, m_name);
}

bool SetCapturePriority(void) {
  static HMODULE avrt = LoadLibrary("Avrt");
  if(!avrt)
    return false;

  static auto pfn = (decltype(&AvSetMmThreadCharacteristicsW))GetProcAddress(avrt, "AvSetMmThreadCharacteristicsW");
  if(!pfn)
    return false;

  DWORD taskID = 0;
  pfn(L"Capture", &taskID);
  return true;
}

void BasicThread::SetThreadPriority(ThreadPriority threadPriority) {
  int nPriority;
  switch(threadPriority) {
  case ThreadPriority::Idle:
    nPriority = THREAD_PRIORITY_IDLE;
    break;
  case ThreadPriority::Lowest:
    nPriority = THREAD_PRIORITY_LOWEST;
    break;
  case ThreadPriority::BelowNormal:
    nPriority = THREAD_PRIORITY_BELOW_NORMAL;
    break;
  case ThreadPriority::Normal:
    nPriority = THREAD_PRIORITY_NORMAL;
    break;
  case ThreadPriority::AboveNormal:
    nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
    break;
  case ThreadPriority::Highest:
    nPriority = THREAD_PRIORITY_HIGHEST;
    break;
  case ThreadPriority::TimeCritical:
    nPriority = THREAD_PRIORITY_TIME_CRITICAL;
    break;
  case ThreadPriority::Multimedia:
    // Special-case handling, we will use the thread characteristics operation directly
    if(SetCapturePriority())
      return;

    // Failed, try to do time-critical priority instead
    nPriority = THREAD_PRIORITY_TIME_CRITICAL;
    break;
  default:
    throw std::runtime_error("Attempted to assign an unrecognized thread priority");
  }

  ::SetThreadPriority(
    m_state->m_thisThread.native_handle(),
    nPriority
  );
}

std::chrono::system_clock::time_point BasicThread::GetCreationTime(void) {
  HANDLE hThread = m_state->m_thisThread.native_handle();
  if(hThread == INVALID_HANDLE_VALUE)
    return std::chrono::system_clock::time_point::min();

  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  auto xyz = std::chrono::system_clock::now();

  int64_t createTime, kernelTime, userTime, exitTime;
  ::GetThreadTimes(hThread, (LPFILETIME) &createTime, (LPFILETIME) &exitTime, (LPFILETIME) &kernelTime, (LPFILETIME) &userTime);
  return
    std::chrono::system_clock::time_point(
      std::chrono::system_clock::duration(createTime)
    );
}

void BasicThread::GetThreadTimes(std::chrono::nanoseconds& kernelTime, std::chrono::nanoseconds& userTime) {
  HANDLE hThread = m_state->m_thisThread.native_handle();

  FILETIME ftCreate, ftExit, ftKernel, ftUser;
  ::GetThreadTimes(hThread, &ftCreate, &ftExit, &ftKernel, &ftUser);
  kernelTime = std::chrono::nanoseconds(100 * (int64_t&) ftKernel);
  userTime = std::chrono::nanoseconds(100 * (int64_t&) ftUser);
}