#include "stdafx.h"
#include "DispatchThunkEventProxy.h"
#include "JunctionBox.h"

void DispatchThunkEventProxy::operator()() {
  eventProxy(dq);
}