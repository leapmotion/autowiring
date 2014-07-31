// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include MEMORY_HEADER

/// <summary>
/// General purpose exception filter, used for tracking exceptions thrown from unit test CoreThreads
/// </summary>
class EnclosedContextExceptionFilter:
  public ContextMember,
  public ExceptionFilter
{
public:
  EnclosedContextExceptionFilter(void) :
    ContextMember("EnclosedContextExceptionFilter"),
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

/// <summary>
/// Provides a test fixture which ensures proper cleanup of a created subcontext
/// </summary>
/// <remarks>
/// Created subcontexts will be terminated and waited upon in the destructor, by default.  Contexts
/// must exit within 250ms of being signalled to stop, or an error will result in order to ensure
/// tests complete in a timely fashion.  If a longer stop duration is desired, you must provide a
/// destructor which performs teardown in a different way.
/// </remarks>
class EnclosedContextTestBase:
  public testing::EmptyTestEventListener
{
private:
  // Weak pointer to the created context, used to detect proper context destruction
  std::weak_ptr<CoreContext> m_createWeak;

public:
  // Base overrides:
  void OnTestStart(const testing::TestInfo& info) override;
  void OnTestEnd(const testing::TestInfo& info) override;
};
