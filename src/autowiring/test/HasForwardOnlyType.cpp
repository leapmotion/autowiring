// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "HasForwardOnlyType.hpp"

// Class defined at file level, so as to prevent the type from being available
// in any compilation unit where a test is defined
class MyForwardedType {};

// And then we provide the ctor
HasForwardOnlyType::HasForwardOnlyType(void) {}

void HasForwardOnlyType::AutoFilter(std::shared_ptr<MyForwardedType>& output) {

}

void InjectForwardOnlyType(CoreContext& ctxt) {
  ctxt.Inject<HasForwardOnlyType>();
}
