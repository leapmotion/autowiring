#include "stdafx.h"
#include "SelfSelectingFixture.h"
#include "MicroBolt.h"

const char s_expectedFixtureName[] = "SelfSelect";
BOLT_TO(SelfSelectingFixture, "Global/IAMANON/SelfSelect")

SelfSelectingFixture::SelfSelectingFixture(void) :
  magic(10010)
{
}

bool SelfSelectingFixture::IsMagicCorrect(void) const {
  return magic == 10010;
}