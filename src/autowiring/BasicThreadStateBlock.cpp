// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "BasicThreadStateBlock.h"
#include "BasicThread.h"

using namespace autowiring;

BasicThreadStateBlock::BasicThreadStateBlock(void) :
  m_priority{ ThreadPriority::Default }
{}

BasicThreadStateBlock::~BasicThreadStateBlock(void)
{}
