#pragma once
#include <list>
#include FUNCTIONAL_HEADER

/// <summary>
/// Maintains a list of lambdas to be invoked when the enclosing object is being destroyed
/// </summary>
class TeardownNotifier
{
public:
  ~TeardownNotifier(void);

protected:
  // Teardown listeners, invoked in sequence when the context is tearing down
  typedef std::list<std::function<void()>> t_teardownListeners;
  t_teardownListeners m_teardownListeners;

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

public:
  /// <summary>
  /// Registers the passed listener to be called when this object is being destroyed
  /// </summary>
  void AddTeardownListener(const std::function<void ()>& listener);
};

