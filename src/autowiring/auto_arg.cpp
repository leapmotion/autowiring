// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "auto_arg.h"
#include "CoreContext.h"

using namespace autowiring;

CoreContext& auto_arg<CoreContext&>::arg(AutoPacket&) {
  return *CoreContext::CurrentContext();
}

std::shared_ptr<CoreContext> auto_arg<std::shared_ptr<CoreContext>>::arg(AutoPacket&) {
  return CoreContext::CurrentContext();
}
