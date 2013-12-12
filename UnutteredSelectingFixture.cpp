#include "stdafx.h"
#include "UnutteredSelectingFixture.h"
#include "MicroBolt.h"

struct SelfSelect {};

ANCHOR_POINT(UnutteredSelectingFixture);
BOLT_TO(SelfSelect, UnutteredSelectingFixture);

UnutteredSelectingFixture::UnutteredSelectingFixture()
{
}

UnutteredSelectingFixture::~UnutteredSelectingFixture()
{
}
