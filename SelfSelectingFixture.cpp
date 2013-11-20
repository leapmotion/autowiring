#include "stdafx.h"
#include "SelfSelectingFixture.h"
#include "MicroBolt.h"

const char s_expectedFixtureName[] = "SelfSelect";
const char s_pipelinename[] = "Global/IAMANON/SelfSelect";

//BOLT_TO(SelfSelectingFixture, SelfSelect);
//static MicroBolt<SelfSelectingFixture, s_expectedFixtureName> mymicro;
//static const MicroBolt<SelfSelectingFixture, s_pipelinename> mymicro;

static AutoRequired<MicroBolt<SelfSelectingFixture, s_pipelinename>> m_b_l_static_no_clash_doo_doo;

SelfSelectingFixture::SelfSelectingFixture(void) :
  magic(10010)
{
}

bool SelfSelectingFixture::IsMagicCorrect(void) const {
  return magic == 10010;
}