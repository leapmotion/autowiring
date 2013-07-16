#pragma once
#include "EnclosedContextTestBase.h"

class CtorDtorListener;
class MyCtorDtorListener1;
class MyCtorDtorListener2;

class DtorCorrectnessTest:
  public EnclosedContextTestBase
{
public:
  DtorCorrectnessTest(void);

  void SetUp(void) override;
  
  AutoFired<CtorDtorListener> cdl;
  AutoRequired<MyCtorDtorListener1> listener1;
  AutoRequired<MyCtorDtorListener2> listener2;
};

