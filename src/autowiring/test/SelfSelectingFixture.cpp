// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "SelfSelectingFixture.hpp"
#include <autowiring/MicroBolt.h>

SelfSelectingFixture::SelfSelectingFixture(void) :
  magic(10010)
{
}

bool SelfSelectingFixture::IsMagicCorrect(void) const {
  return magic == 10010;
}
