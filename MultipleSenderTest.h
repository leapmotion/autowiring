#pragma once
#include "EnclosedContextTestBase.h"

class MultiSender;

class MultipleSenderTest:
  public EnclosedContextTestBase
{
public:
  MultipleSenderTest(void);
  AutoRequired<MultiSender> m_sender;
};

