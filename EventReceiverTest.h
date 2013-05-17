#ifndef _DEFERRED_INVOCATION_TEST_H
#define _DEFERRED_INVOCATION_TEST_H
#include "EnclosedContextTestBase.h"

class SimpleReceiver;
class SimpleSender;

class EventReceiverTest:
  public EnclosedContextTestBase
{
public:
  EventReceiverTest(void);

protected:
  AutoRequired<SimpleReceiver> receiver;
  AutoRequired<SimpleSender> sender;
};

#endif