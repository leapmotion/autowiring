// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "EnclosedContextTestBase.hpp"

class CtorDtorListener;
template<int i>
class MyCtorDtorListenerN;

class DtorCorrectnessTest:
  public EnclosedContextTestBase
{
public:
  DtorCorrectnessTest(void);

  void SetUp(void) override;
  
  AutoFired<CtorDtorListener> cdl;
  AutoRequired<MyCtorDtorListenerN<1>> listener1;
  AutoRequired<MyCtorDtorListenerN<2>> listener2;
};
