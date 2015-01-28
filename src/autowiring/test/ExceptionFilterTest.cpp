// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "TestFixtures/ThrowsWhenFired.hpp"
#include "TestFixtures/ThrowsWhenRun.hpp"
#include <autowiring/ExceptionFilter.h>
#include <stdexcept>
#include <sstream>
#include <iostream>

class ExceptionFilterTest:
  public testing::Test
{};

using namespace std;

string FormatMessage(int value) {
  std::stringstream sstr;
  sstr << "custom_exception: " << value;
  return sstr.str();
}

/// <summary>
/// Exception type which can track its own destruction
/// </summary>
class tracking_exception:
  public std::exception
{
public:
  tracking_exception(int) {
    s_count++;
  }
  tracking_exception(const tracking_exception& rhs) {
    s_count++;
  }
  ~tracking_exception(void) throw() {
    s_count--;
  }

  static size_t s_count;
};

size_t tracking_exception::s_count = 0;

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

  virtual void Filter(void) override {
    m_hit = true;
    try {
      throw;
    } catch(tracking_exception&) {
    } catch(custom_exception& custom) {
      EXPECT_EQ(100, custom.m_value) << "A filtered custom exception did not have the expected member field value";
      m_specific = true;
    } catch(...) {
      m_generic = true;
    }
  }

  virtual void Filter(const JunctionBoxBase* pJunctionBox, Object* pRecipient) override {
    m_hit = true;
    try {
      throw;
    } catch(custom_exception& custom) {
      EXPECT_EQ(200, custom.m_value) << "A fired exception did not have the expected value, probable copy malfunction";
      m_fireSpecific = true;
    } catch(...) {
      m_generic = true;
    }
  }
};

TEST_F(ExceptionFilterTest, AUTOTHROW_ExceptionDestruction) {
  // Add the exception filter type to the context first
  AutoRequired<GenericFilter> filter;

  // Now add something that will throw when it's run:
  AutoRequired<ThrowsWhenRun<tracking_exception>> thrower;

  // Run:
  AutoCurrentContext()->Initiate();
  thrower->Wait();

  // Verify that the exception was destroyed the correct number of times:
  EXPECT_EQ(0UL, tracking_exception::s_count) << "Exception was not destroyed the correct number of times";
}

TEST_F(ExceptionFilterTest, CheckThrowThrow) {
  class example {
  public:
    example() {
      throw std::exception();
    }
  };

  EXPECT_THROW(throw example(), std::exception) << "An exception type which throws from its ctor did not throw the expected type";
}

TEST_F(ExceptionFilterTest, AUTOTHROW_ThreadThrowsCheck) {
  // Add the exception filter type to the context first
  AutoRequired<GenericFilter> filter;

  // Now add something that will throw when it's run:
  AutoRequired<ThrowsWhenRun<custom_exception>> thrower;

  // Wait for the thrower to terminate, should be pretty fast:
  AutoCurrentContext()->Initiate();
  thrower->Wait();

  // Hopefully the filter got hit in the right spot:
  EXPECT_TRUE(filter->m_hit) << "Filter was not invoked for a thrown exception";
  EXPECT_TRUE(filter->m_specific) << "Filter did not correctly detect the exception type";
  EXPECT_FALSE(filter->m_generic) << "Filter did not correctly filter out a specific exception";
}

TEST_F(ExceptionFilterTest, SimpleFilterCheck) {
  AutoCurrentContext()->Initiate();

  // Firing will occur at the parent context scope:
  AutoFired<ThrowingListener> broadcaster;

  // Add the generic filter:
  AutoRequired<GenericFilter> filter;

  // Add a context member which will throw when its event is fired, and a firing class:
  AutoRequired<ThrowsWhenFired<>> fireThrower;

  // Add something to fire the exception:
  EXPECT_NO_THROW(broadcaster(&ThrowingListener::DoThrow)());

  // Verify that the exception was filtered properly by the generic filter:
  EXPECT_TRUE(filter->m_fireSpecific) << "Filter was not invoked on a Fired exception";
}

TEST_F(ExceptionFilterTest, FireContainmentCheck) {
  // Initiate parent context first
  AutoCurrentContext()->Initiate();

  // Firing will occur at the parent context scope:
  AutoFired<ThrowingListener> broadcaster;

  // Create a subcontext and add the fire thrower to it:
  AutoCreateContext ctxt;
  ctxt->Initiate();
  ctxt->Inject<ThrowsWhenFired<>>();

  // Now cause the exception to occur:
  EXPECT_NO_THROW(broadcaster(&ThrowingListener::DoThrow)());

  // Verify that the context containing the fire thrower was torn down:
  EXPECT_TRUE(ctxt->IsShutdown()) << "An unhandled exception from a fire call in a context should have signalled it to stop";

  // Verify that the parent context was protected:
  EXPECT_FALSE(AutoCurrentContext()->IsShutdown()) << "An unhandled exception incorrectly terminated a parent context";
}

TEST_F(ExceptionFilterTest, AUTOTHROW_EnclosedThrowCheck) {
  AutoCurrentContext()->Initiate();

  // Create our listener:
  AutoRequired<GenericFilter> filter;

  // Now the subcontext:
  AutoCreateContext subCtxt;
  CurrentContextPusher pshr(subCtxt);

  // Create and start:
  AutoRequired<ThrowsWhenRun<custom_exception>> runThrower;
  subCtxt->Initiate();

  // Wait for the exception to get thrown:
  ASSERT_TRUE(subCtxt->Wait(std::chrono::seconds(5))) << "Context did not terminate in a timely fashion";

  // Verify that the filter caught the exception:
  EXPECT_TRUE(filter->m_hit) << "Filter operating in a superior context did not catch an exception thrown from a child context";
}

TEST_F(ExceptionFilterTest, VerifyThrowingRecipients) {
  AutoCurrentContext()->Initiate();

  // Create a pair of classes which throw exceptions:
  AutoRequired<ThrowsWhenFired<custom_exception, 200>> v200;
  AutoRequired<ThrowsWhenFired<custom_exception, 201>> v201;

  // Now try to throw:
  AutoFired<ThrowingListener> tl;
  tl(&ThrowingListener::DoThrow)();

  // Verify that BOTH are hit:
  EXPECT_TRUE(v200->hit && v201->hit) << "Expected all receivers of a fired event will be processed, even if some throw exceptions";
}

TEST_F(ExceptionFilterTest, ExceptionFirewall) {
  AutoCurrentContext()->Initiate();

  AutoRequired<ThrowsWhenFired<custom_exception,200>> v200;

  // Try to throw, verify the return value.  The value should be false, because this particular type always
  // throws an exception in response to the receipt of an event.
  AutoFired<ThrowingListener> tl;
  ASSERT_FALSE(tl(&ThrowingListener::DoThrow)()) << "An exception event was not properly indicated to an event firer";
}

TEST_F(ExceptionFilterTest, VerifySimpleConfinement) {
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  // Create a subcontext where the errant recipients will live:
  AutoCreateContext child;
  child->Initiate();
  child->Inject<ThrowsWhenFired<custom_exception, 200>>();

  Autowired<ThrowsWhenFired<custom_exception, 200>> twf;
  ASSERT_FALSE(twf) << "A member injected into a child context was incorrectly scoped at the parent context";

  // Cause the child context to throw an exception:
  AutoFired<ThrowingListener> tl;
  tl(&ThrowingListener::DoThrow)();

  // Verify that the parent scope wasn't incorrectly terminated:
  EXPECT_FALSE(ctxt->IsShutdown()) << "Parent scope was terminated incorrectly due to an exception sourced by a child context";

  // Verify that the child scope was terminated as expected:
  EXPECT_TRUE(child->IsShutdown()) << "An event recipient in a child scope threw an exception and the child context was not correctly terminated";
}

TEST_F(ExceptionFilterTest, NoRecursiveShutdowns) {

}
