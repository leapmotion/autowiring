#include "stdafx.h"
#include "AutowiringEnclosure.h"

struct TestInfoProxy {
  TestInfoProxy(const testing::TestInfo& info) :
    m_info(info)
  {}

  const testing::TestInfo& m_info;
};

void AutowiringEnclosure::OnTestStart(const testing::TestInfo& info) {
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

void AutowiringEnclosure::OnTestEnd(const testing::TestInfo& info) {
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

  static const char sc_autothrow[] = "AUTOTHROW_";
  if(!strncmp(sc_autothrow, info.name(), ARRAYCOUNT(sc_autothrow) - 1))
    // Throw expected, end here
    return;

  // If an exception occurred somewhere, report it:
  ASSERT_FALSE(ecef->m_excepted)
    << "An unhandled exception occurred in this context" << std::endl
    << "[" << (ecef->m_ti ? ecef->m_ti->name() : "unknown") << "] " << ecef->m_what;
}