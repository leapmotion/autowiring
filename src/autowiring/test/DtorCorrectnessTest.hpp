// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

class CtorDtorListener;
template<int i>
class MyCtorDtorListenerN;

class DtorCorrectnessTest:
  public testing::Test
{
public:
  DtorCorrectnessTest(void);

  void SetUp(void) override;
  
  AutoFired<CtorDtorListener> cdl;
  AutoRequired<MyCtorDtorListenerN<1>> listener1;
  AutoRequired<MyCtorDtorListenerN<2>> listener2;
};
