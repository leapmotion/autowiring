// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include <autowiring/autowiring.h>
#include <autowiring/CoreThread.h>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

class MyBasicThread:
  public BasicThread
{
public:
  virtual void Run() override {
    for (auto x : {0, 1, 2 ,3}) {
      std::cout << "MyBasicThread: " << x << "\n";
    }
  }
};

class MyCoreThread:
  public CoreThread
{
public:
  void AddToQueue(int x) {
    *this += [x] {
      std::cout << "MyCoreThread: " << x << "\n";
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    };
  }
};

int main(){

  // The 2 main thread classes in Autowiring are the BasicThread and CoreThread.
  // Classes that inherit from these types will have thread capabilities
  // Both start when their enclosing context is 'initiated'. Threads injected
  // after the context is initiated will start immediatly

  AutoRequired<MyBasicThread> myBasic;

  AutoCurrentContext ctxt;
  ctxt->Initiate(); // myBasic->Run() starts now in its own thread

  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  std::cout << "injecting a CoreThread" << std::endl;

  // Types inheriting from CoreThread implement a dispatch queue in their 'run()'
  // function. Lambdas can be appended with operator+=

  AutoRequired<MyCoreThread> myCore;
  myCore->AddToQueue(42);
  myCore->AddToQueue(1337);

  *myCore += []{
    std::cout << "This gets run after '1337'" << std::endl;
  };

  // This should be run before 'myCore' is finished
  std::cout << "This thread is faster\n";

  // This will wait for all outstanding threads to finish before terminating the context
  ctxt->SignalShutdown(true);
}

