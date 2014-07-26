// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once

#include <autowiring/Autowired.h>
#include <gtest/gtest.h>
#include MEMORY_HEADER

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
  public testing::Test
{
public:
  EnclosedContextTestBase(void):
    m_pshr(m_create)
  {
    m_createWeak = m_create;
  }

  ~EnclosedContextTestBase(void) {
    // Only attempt teardown if it hasn't already happened:
    auto ctxt = m_createWeak.lock();
    if(!ctxt)
      return;

    ctxt->SignalShutdown();

    // Do not allow teardown to take more than a millisecond
    if(!ctxt->Wait(std::chrono::milliseconds(250))) {
      // Critical error--took too long to tear down
      assert(false);
    }
  }

protected:
  // The context proper.  This is automatically assigned as the current
  // context when SetUp is invoked.
  AutoCreateContext m_create;

  // Currency pusher:
  CurrentContextPusher m_pshr;
 
private:
  // Weak pointer to the created context, used to detect proper context destruction
  std::weak_ptr<CoreContext> m_createWeak;

public:
  /// <returns>
  /// True if there are no outstanding pointers to the created context, except the one held by base
  /// </returns>
  bool IsContextClean(void) const {return m_create.unique();}
};
