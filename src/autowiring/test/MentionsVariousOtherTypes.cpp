// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "MentionsVariousOtherTypes.hpp"
#include <autowiring/autowiring.h>

MentionsVariousOtherTypes::MentionsVariousOtherTypes(void) {
  AutoRequired<AutoPacketFactory>();
}
