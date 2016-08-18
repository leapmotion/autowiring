// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "C++11/cpp11.h"
#include <atomic>
#include RVALUE_HEADER

namespace autowiring {

/// <summary>
/// Maintains a list of lambdas to be invoked when the enclosing object is being destroyed
/// </summary>
class TeardownNotifier
{
public:
  TeardownNotifier(void);
  ~TeardownNotifier(void);

protected:
  struct EntryBase
  {
  public:
    virtual ~EntryBase(void) {}
    virtual void operator()(void) = 0;

    EntryBase* pFlink = nullptr;
  };

  template<class Fx>
  struct Entry:
    EntryBase
  {
    Entry(Fx&& fx):
      fx(std::forward<Fx&&>(fx))
    {}

    Fx fx;

    void operator()(void) override { fx(); }
  };

  // Teardown listeners, invoked in sequence when the context is tearing down
  std::atomic<EntryBase*> m_pFirstTeardownListener;

  /// <summary>
  /// May be invoked prospectively by a derived instance to prematurely notify teardown listeners
  /// </summary>
  /// <remarks>
  /// This method may be useful when it's necessary to notify teardown listeners while an object is
  /// still being destroyed.
  ///
  /// Note that this method will clear the list of teardown listeners.  New listeners may potentially
  /// be registered even after this method is called.  The teardown notifier will ensure these
  /// listeners are correctly called.
  ///
  /// This method is idempotent iff no new teardown listeners are added between calls.
  /// </remarks>
  void NotifyTeardownListeners(void);

  /// <summary>
  /// Runtime version of AddTeardownListener
  /// </summary>
  void AddTeardownListenerInternal(EntryBase* listener);

public:
  /// <summary>
  /// Registers the passed listener to be called when teardown is occurring for this object
  /// </summary>
  /// <remarks>
  /// Teardown can mean different things depending on the object.  Generally speaking, teardown occurs
  /// when an object is executing some kind of cleanup operation, or is exiting its operational state.
  /// Objects which maintain an internal thread enter teardown when their thread is exiting.  Other
  /// objects enter teardown when they are being destroyed.
  ///
  /// Regardless, the teardown notifier provides a strong guarantee that the listener will be called
  /// during teardown, or at some point afterwards, by performing teardown notification during the
  /// destructor.  Unless otherwise stated by the object in question, however, the object should be
  /// treated as invalid at the time of notification.
  /// </remarks>
  template<class Fx>
  void AddTeardownListener(Fx&& listener) {
    AddTeardownListenerInternal(new Entry<Fx>(std::forward<Fx&&>(listener)));
  }
};
}
