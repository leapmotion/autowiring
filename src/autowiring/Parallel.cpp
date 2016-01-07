// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "Parallel.h"
#include "autowiring.h"

using namespace autowiring;

void parallel_iterator<void>::operator++(int) {
  m_parent.Pop<void>();
}

parallel::parallel(void):
  m_ctxt(AutoCurrentContext{})
{}

parallel::parallel(CoreContext& ctxt) :
  m_ctxt(ctxt.shared_from_this())
{}
