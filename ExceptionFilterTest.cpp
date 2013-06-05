#include "stdafx.h"
#include "ExceptionFilterTest.h"
#include "EventReceiver.h"
#include "ExceptionFilter.h"
#include <stdexcept>

class custom_exception:
  public std::exception
{
public:
  custom_exception(int value):
    m_value(value)
    {}

  int m_value;
};

class ThrowingListener:
  public EventReceiver
{
public:
  virtual void DoThrow(void) = 0;
};

class ThrowsWhenRun:
  public CoreThread
{
public:
  ThrowsWhenRun(void) {
    Ready();
  }

  void Run(void) override {
    throw custom_exception(100);
  }
};

class ThrowsWhenFired:
  public ThrowingListener
{
public:
  void DoThrow(void) override {
    throw custom_exception(200);
  }
};

class ThrowingBroadcaster:
  public EventManager<ThrowingListener>
{
public:
  using EventManager<ThrowingListener>::Fire;
};

class GenericFilter:
  public ExceptionFilter
{
public:
  GenericFilter(void):
    m_hit(false),
    m_specific(false),
    m_generic(false),
    m_fireSpecific(false)
  {
  }

  bool m_hit;
  bool m_specific;
  bool m_generic;
  bool m_fireSpecific;

  virtual void Filter(const std::function<void()>& rethrower) override {
    m_hit = true;
    try {
      rethrower();
    } catch(custom_exception& custom) {
      EXPECT_EQ(100, custom.m_value) << "A filtered custom exception did not have the expected member field value";
      m_specific = true;
    } catch(...) {
      m_generic = true;
    }
  }

  virtual void Filter(const std::function<void()>& rethrower, const EventManagerBase* pSender, EventReceiver* pRecipient) override {
    m_hit = true;
    try {
      rethrower();
    } catch(custom_exception& custom) {
      EXPECT_EQ(200, custom.m_value) << "A fired exception did not have the expected value, probable copy malfunction";
      m_fireSpecific = true;
    } catch(...) {
      m_generic = true;
    }
  }
};

#if PLATFORM_RETHROW_EXISTS
  #define ONLY_PLATFORM_RETHROW_EXISTS(x) x
#else
  #define ONLY_PLATFORM_RETHROW_EXISTS(x) DISABLED_##x
#endif

TEST_F(ExceptionFilterTest, ONLY_PLATFORM_RETHROW_EXISTS(ThreadThrowsCheck)) {
  // Add the exception filter type to the context first
  AutoRequired<GenericFilter> filter;

  // Now add something that will throw when it's run:
  AutoRequired<ThrowsWhenRun> thrower;

  // Wait for the thrower to terminate, should be pretty fast:
  m_create->InitiateCoreThreads();
  thrower->Wait();

  // Hopefully the filter got hit in the right spot:
  EXPECT_TRUE(filter->m_hit) << "Filter was not invoked for a thrown exception";
  EXPECT_TRUE(filter->m_specific) << "Filter did not correctly detect the exception type";
  EXPECT_FALSE(filter->m_generic) << "Filter did not correctly filter out a specific exception";
}

TEST_F(ExceptionFilterTest, ONLY_PLATFORM_RETHROW_EXISTS(FireThrowsCheck)) {
  // Add the generic filter:
  AutoRequired<GenericFilter> filter;

  // Add a context member which will throw when its event is fired, and a firing class:
  AutoRequired<ThrowsWhenFired> fireThrower;

  // Add something to fire the exception:
  AutoRequired<ThrowingBroadcaster> broadcaster;
  broadcaster->Fire(&ThrowingListener::DoThrow)();

  // Verify that the exception was filtered properly by the generic filter:
  EXPECT_TRUE(filter->m_fireSpecific) << "Filter was not invoked on a Fired exception";
}