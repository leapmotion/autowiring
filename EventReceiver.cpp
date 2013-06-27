#include "stdafx.h"
#include "EventReceiver.h"
#include "CoreThread.h"

bool CoreThreadCanAccept(CoreThread* pThread) {
  return pThread->CanAccept();
}