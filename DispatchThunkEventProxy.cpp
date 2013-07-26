#include "stdafx.h"
#include "DispatchThunkEventProxy.h"
#include "EventSender.h"

void DispatchThunkEventProxy::operator()() {
  eventProxy(dq);
}