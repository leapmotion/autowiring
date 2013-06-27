// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "SharedPtrWrap.h"

std::shared_ptr<Object> SharedPtrWrapBase::AsObject() {
  return std::shared_ptr<Object>();
}
