// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "SelfSelectingFixture.hpp"
#include <autowiring/MicroBolt.h>

SelfSelectingFixture::SelfSelectingFixture(void) :
  magic(10010)
{
}

bool SelfSelectingFixture::IsMagicCorrect(void) const {
  return magic == 10010;
}
