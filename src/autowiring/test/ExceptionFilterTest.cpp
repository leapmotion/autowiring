// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
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
  bool m_hit = false;
  bool m_specific = false;
  bool m_generic = false;
  bool m_fireSpecific = false;

  virtual void Filter(void) override {
    m_hit = true;
    try {
      throw;
    } catch(tracking_exception&) {
    } catch(custom_exception& custom) {
      ASSERT_EQ(100, custom.m_value) << "A filtered custom exception did not have the expected member field value";
      m_specific = true;
    } catch(...) {
      m_generic = true;
    }
  }

  virtual void Filter(const JunctionBoxBase* pJunctionBox, CoreObject* pRecipient) override {
    m_hit = true;
    try {
      throw;
    } catch(custom_exception& custom) {
      ASSERT_EQ(200, custom.m_value) << "A fired exception did not have the expected value, probable copy malfunction";
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
  ASSERT_EQ(0UL, tracking_exception::s_count) << "Exception was not destroyed the correct number of times";
}

TEST_F(ExceptionFilterTest, CheckThrowThrow) {
  class example {
  public:
    example() {
      throw std::exception();
    }
  };

  ASSERT_THROW(throw example(), std::exception) << "An exception type which throws from its ctor did not throw the expected type";
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
  ASSERT_TRUE(filter->m_hit) << "Filter was not invoked for a thrown exception";
  ASSERT_TRUE(filter->m_specific) << "Filter did not correctly detect the exception type";
  ASSERT_FALSE(filter->m_generic) << "Filter did not correctly filter out a specific exception";
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
  ASSERT_TRUE(filter->m_hit) << "Filter operating in a superior context did not catch an exception thrown from a child context";
}
