#pragma once
#include "EnclosedContextTestBase.h"

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

