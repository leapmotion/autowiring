#include "stdafx.h"
#include "SelfSelectingFixture.h"
#include "MicroBolt.h"

SelfSelectingFixture::SelfSelectingFixture(void) :
  magic(10010)
{
}

bool SelfSelectingFixture::IsMagicCorrect(void) const {
  return magic == 10010;
}