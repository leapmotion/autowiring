// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _ENCLOSED_CONTEXT_TEST_BASE_H
#define _ENCLOSED_CONTEXT_TEST_BASE_H
#include "Autowired.h"
#include <gtest/gtest.h>
#include <memory>

class EnclosedContextTestBase:
  public testing::Test
{
public:
  EnclosedContextTestBase(void);
  ~EnclosedContextTestBase(void);

protected:
  // The context proper.  This is automatically assigned as the current
  // context when SetUp is invoked.
  AutoCreateContext m_create;

  // Currency pusher:
  CurrentContextPusher m_pshr;
};

#endif
