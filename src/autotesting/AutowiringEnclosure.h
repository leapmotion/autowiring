// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <autowiring/autowiring.h>
#include <autowiring/AutowiringDebug.h>
#include <autowiring/demangle.h>
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
      autowiring::dbg::PrintRunnables(os, lhs.ctxt);
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
  // The context that is current for the test being run right now
  std::shared_ptr<CoreContext> m_ctxt;

  // Base overrides:
  void OnTestStart(const testing::TestInfo& info) override {

    // Set global context current first, then initiate
    AutoGlobalContext global;
    global->SetCurrent();
    global->Initiate();

    // The context proper.  This is automatically assigned as the current
    // context when SetUp is invoked.
    m_ctxt = AutoCreateContext{};
    m_ctxt->Inject<TestInfoProxy>(info);

    // Add exception filter in this context:
    m_ctxt->Inject<AutowiringEnclosureExceptionFilter>();

    // Now make it current and let the test run:
    m_ctxt->SetCurrent();
  }

  void OnTestEnd(const testing::TestInfo& info) override {
    // TLS clear to prevent leaks
    CoreContext::EvictCurrent();

    // Current context moved to local so we can ensure we free it
    auto ctxt = std::move(m_ctxt);
    m_ctxt = {};

    // Global initialization tests are special, we don't bother checking closure principle on them:
    if (!strcmp("GlobalInitTest", info.test_case_name()))
      return;

    // Try to tear down the test context enclosure:
    ctxt->SignalShutdown();

    {
      // Verify we can grab the test case back out and that the pointer is correct:
      AutowiredFast<AutowiringEnclosureExceptionFilter> ecef{ ctxt };

      // Need to make sure we got back our exception filter before continuing:
      ASSERT_TRUE(ecef.IsAutowired()) << "Failed to find the enclosed context exception filter; unit test may have incorrectly reset the enclosing context before returning";

      // Do not allow teardown to take more than 5 seconds.  This is considered a "timely teardown" limit.
      // If it takes more than this amount of time to tear down, the test case itself should invoke SignalShutdown
      // and Wait itself with the extended teardown period specified.
      ASSERT_TRUE(ctxt->Wait(std::chrono::seconds(5))) << "Test case took too long to tear down, unit tests running after this point are untrustworthy.  Runnable dump:\n" << autowiring::testing::hung{ *ctxt };

      // Global context should return to quiescence:
      if (!allowGlobalReferences)
        ASSERT_TRUE(AutoGlobalContext()->Quiescent(std::chrono::seconds(5))) << "Contexts took too long to release all references to the global context";

      static const char sc_autothrow [] = "AUTOTHROW_";
      if (strncmp(sc_autothrow, info.name(), sizeof(sc_autothrow) - 1)) {
        // If an exception occurred somewhere, report it:
        ASSERT_FALSE(ecef->m_excepted)
          << "An unhandled exception occurred in this context" << std::endl
          << "[" << (ecef->m_ti ? autowiring::demangle(*ecef->m_ti) : "unknown") << "] " << ecef->m_what;
      }
    }

    // No more references to this context except for the pointer we hold ourselves
    ASSERT_TRUE(ctxt.unique()) << "Detected a dangling context reference after test termination, context may be leaking";
    ctxt = {};
  }
};
