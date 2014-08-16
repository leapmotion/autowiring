// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/autowiring.h>
#include MEMORY_HEADER

#ifndef GTEST_INCLUDE_GTEST_GTEST_H_
#error Please include your version of gtest.h before including the autowiring enclosure
#endif

/// <summary>
/// General purpose exception filter, used for tracking exceptions thrown from unit test CoreThreads
/// </summary>
class AutowiringEnclosureExceptionFilter:
  public ContextMember,
  public ExceptionFilter
{
public:
  AutowiringEnclosureExceptionFilter(void) :
    ContextMember("AutowiringEnclosureExceptionFilter"),
    m_excepted(false)
  {}

  bool m_excepted;

  const std::type_info* m_ti;
  std::string m_what;

  void Filter(void) override {
    m_excepted = true;
    try {
      throw;
    }
    catch(std::exception& except) {
      m_ti = &typeid(except);
      m_what = except.what();
    }
    catch(...) {}
  }
};

struct TestInfoProxy {
  TestInfoProxy(const testing::TestInfo& info) :
    m_info(info)
  {}

  const testing::TestInfo& m_info;
};

/// <summary>
/// Provides a test fixture which ensures proper cleanup of a created subcontext
/// </summary>
/// <remarks>
/// Created subcontexts will be terminated and waited upon in the destructor, by default.  Contexts
/// must exit within 250ms of being signalled to stop, or an error will result in order to ensure
/// tests complete in a timely fashion.  If a longer stop duration is desired, you must provide a
/// destructor which performs teardown in a different way.
/// </remarks>
class AutowiringEnclosure:
  public testing::EmptyTestEventListener
{
private:
  // Weak pointer to the created context, used to detect proper context destruction
  std::weak_ptr<CoreContext> m_createWeak;

public:
  // Base overrides:
  void OnTestStart(const testing::TestInfo& info) override {
    AutoRequired<AutowiringEnclosureExceptionFilter> filter;

    // The context proper.  This is automatically assigned as the current
    // context when SetUp is invoked.
    AutoCreateContext create;
    create->Construct<TestInfoProxy>(info);

    // Add exception filter in this context:
    create->Inject<AutowiringEnclosureExceptionFilter>();

    // Now make it current and let the test run:
    create->SetCurrent();
  }

  void OnTestEnd(const testing::TestInfo& info) override {
    // Verify we can grab the test case back out and that the pointer is correct:
    Autowired<TestInfoProxy> ti;
    Autowired<AutowiringEnclosureExceptionFilter> ecef;
    auto ctxt = ecef ? ecef->GetContext() : nullptr;

    // Unconditionally reset the global context as the current context
    AutoGlobalContext()->SetCurrent();

    // Global initialization tests are special, we don't bother checking closure principle on them:
    if(!strcmp("GlobalInitTest", info.test_case_name()))
      return;

    // Need to make sure we got back our exception filter before continuing:
    ASSERT_TRUE(ecef.IsAutowired()) << "Failed to find the enclosed context exception filter; unit test may have incorrectly reset the enclosing context before returning";

    // Now try to tear down the test context enclosure:
    ctxt->SignalShutdown();

    // Do not allow teardown to take more than 250 milliseconds or so
    if(!ctxt->Wait(std::chrono::milliseconds(250))) {
      // Critical error--took too long to tear down
      assert(false);
    }

    static const char sc_autothrow [] = "AUTOTHROW_";
    if(!strncmp(sc_autothrow, info.name(), sizeof(sc_autothrow) - 1))
      // Throw expected, end here
      return;

    // If an exception occurred somewhere, report it:
    ASSERT_FALSE(ecef->m_excepted)
      << "An unhandled exception occurred in this context" << std::endl
      << "[" << (ecef->m_ti ? ecef->m_ti->name() : "unknown") << "] " << ecef->m_what;
  }
};
