// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include <autowiring/Autowired.h>
#include <iostream>
#include <memory>

/////////////////////////////////////////////////////////////////////
//                                                                 //
//                        Autowiring Events                        //
//                                                                 //
/////////////////////////////////////////////////////////////////////

// Autowiring events are just calls on member functions of in a context.
// All context members that implement function pointer you "fire" will be called

class MyEvent {
public:
  virtual void myFunction(int) = 0;
};

// Will receive MyEvent::myFunction or FooEvent::myFunction events
class FooEvent:
  public MyEvent
{
public:
  FooEvent():
    m_secret(0)
  {}
  
  void myFunction(int i) override {
    m_secret = i;
  }
  
  int getSecret(){
    return m_secret;
  }
private:
  int m_secret;
};

// Will receive MyEvent::myFunction or BarEvent::myFunction events
class BarEvent:
  public MyEvent
{
public:
  BarEvent():
    m_secret(0)
  {}
  
  void myFunction(int i) override {
    m_secret = i;
  }
  
  int getSecret(){
    return m_secret;
  }
private:
  int m_secret;
};

int main(){
  AutoCurrentContext ctxt;
  
  // A context must be initiated before events can be received. Similar to CoreThread
  ctxt->Initiate();
  
  // This creates an proxy object that can fire MyEvent::* events
  AutoFired<MyEvent> eventFirer;
  
  // Inject receiver types into current context
  AutoRequired<FooEvent> foo;
  AutoRequired<BarEvent> bar;
  std::cout << "Foo should be 0: " << foo->getSecret() << std::endl;
  std::cout << "Bar Should be 0: " << bar->getSecret() << std::endl;
  
  // Fire event, this should set m_secret on both foo and bar
  eventFirer(&MyEvent::myFunction)(42);
  std::cout << "Foo should be 42: " << foo->getSecret() << std::endl;
  std::cout << "Bar should be 42: " << bar->getSecret() << std::endl;
  
  // You can also manually fire events on a context with `Invoke`
  // Since the function pointer is to `BarEvent`, `FooEvent` won't receive the event
  ctxt->Invoke(&BarEvent::myFunction)(77);
  std::cout << "Foo should be 42: " << foo->getSecret() << std::endl;
  std::cout << "Bar should be 77: " << bar->getSecret() << std::endl;
}
