// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/autowiring.h>
#include "demangle.h"
#include <autowiring/at_exit.h>
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

namespace autowiring {
  namespace testing {
    struct hung {
      CoreContext& ctxt;
    };

    std::ostream& operator<<(std::ostream& os, const hung& lhs) {
      auto runnables = lhs.ctxt.GetRunnables();
      for (CoreRunnable* runnable : runnables)
        if (runnable->IsRunning())
          os << autowiring::demangle(typeid(*runnable)) << '\n';
      return os;
    }
  }
}

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
public:
  /// <summary>
  /// Constructs a new AutowiringEnclosure
  /// </summary>
  /// <param name="allowGlobalReferences">
  /// Prevents the enclosure from ensuring that the global context returns to a quiescent state
  /// </param>
  AutowiringEnclosure(bool allowGlobalReferences = false) :
    allowGlobalReferences(allowGlobalReferences)
  {}

  const bool allowGlobalReferences;

  // Base overrides:
  void OnTestStart(const testing::TestInfo& info) override {
    AutoRequired<AutowiringEnclosureExceptionFilter> filter;

    // Set global context current first, then initiate
    AutoGlobalContext global;
    global->SetCurrent();
    global->Initiate();

    // The context proper.  This is automatically assigned as the current
    // context when SetUp is invoked.
    AutoCreateContext create;
    create->Inject<TestInfoProxy>(info);

    // Add exception filter in this context:
    create->Inject<AutowiringEnclosureExceptionFilter>();

    // Now make it current and let the test run:
    create->SetCurrent();
  }

  void OnTestEnd(const testing::TestInfo& info) override {
    auto setglobal = MakeAtExit([] {
      // Unconditionally nullify the global context as the current context
      CoreContext::EvictCurrent();
    });

    // Verify we can grab the test case back out and that the pointer is correct:
    Autowired<AutowiringEnclosureExceptionFilter> ecef;

    // Global initialization tests are special, we don't bother checking closure principle on them:
    if(!strcmp("GlobalInitTest", info.test_case_name()))
      return;

    // Need to make sure we got back our exception filter before continuing:
    ASSERT_TRUE(ecef.IsAutowired()) << "Failed to find the enclosed context exception filter; unit test may have incorrectly reset the enclosing context before returning";

    // Now try to tear down the test context enclosure:
    auto ctxt = ecef ? ecef->GetContext() : nullptr;
    ctxt->SignalShutdown();

    // Do not allow teardown to take more than 5 seconds.  This is considered a "timely teardown" limit.
    // If it takes more than this amount of time to tear down, the test case itself should invoke SignalShutdown
    // and Wait itself with the extended teardown period specified.
    ASSERT_TRUE(ctxt->Wait(std::chrono::seconds(5))) << "Test case took too long to tear down, unit tests running after this point are untrustworthy.  Hung runnables:\n" << autowiring::testing::hung{ *ctxt };

    // Global context should return to quiescence:
    if (!allowGlobalReferences)
      ASSERT_TRUE(AutoGlobalContext()->Quiescent(std::chrono::seconds(5))) << "Contexts took too long to release all references to the global context";

    // And no more references to this context, except the current context and the pointer itself
    ASSERT_GE(2, ctxt.use_count()) << "Detected a dangling context reference after test termination, context may be leaking";

    static const char sc_autothrow [] = "AUTOTHROW_";
    if(!strncmp(sc_autothrow, info.name(), sizeof(sc_autothrow) - 1))
      // Throw expected, end here
      return;

    // If an exception occurred somewhere, report it:
    ASSERT_FALSE(ecef->m_excepted)
      << "An unhandled exception occurred in this context" << std::endl
      << "[" << (ecef->m_ti ? autowiring::demangle(*ecef->m_ti) : "unknown") << "] " << ecef->m_what;
  }
};
