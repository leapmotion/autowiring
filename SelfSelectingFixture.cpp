#include "stdafx.h"
#include "SelfSelectingFixture.h"
#include "MicroBolt.h"

BOLT_TO_AND_ANCHOR_POINT(SelfSelect, SelfSelectingFixture);

SelfSelectingFixture::SelfSelectingFixture(void) :
  magic(10010)
{
}

bool SelfSelectingFixture::IsMagicCorrect(void) const {
  return magic == 10010;
}