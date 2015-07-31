// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "SatCounter.h"
#include <sstream>

void SatCounter::ThrowRepeatedCallException(void) const {
  std::stringstream ss;
  ss << "Repeated call to " << autowiring::demangle(m_type);
  throw std::runtime_error(ss.str());
}
