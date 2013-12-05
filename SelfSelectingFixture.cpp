#include "stdafx.h"
#include "SelfSelectingFixture.h"
#include "MicroBolt.h"


BOLT_TO(SelfSelect, SelfSelectingFixture) //gotta go back and make sure this can cross project boundaries once finished

SelfSelectingFixture::SelfSelectingFixture(void) :
  magic(10010)
{
}

bool SelfSelectingFixture::IsMagicCorrect(void) const {
  return magic == 10010;
}