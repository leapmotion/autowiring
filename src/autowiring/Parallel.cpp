// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "Parallel.h"

using namespace autowiring;

void parallel_iterator<void>::operator++(int) {
  m_parent.Pop<void>();
}
