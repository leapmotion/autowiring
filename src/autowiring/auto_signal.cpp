// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "auto_signal.h"
#include "SlotInformation.h"

using namespace autowiring;
using namespace autowiring::internal;

void signal_base::operator-=(const registration_t& reg) {
  *this -= reg.entry;
}