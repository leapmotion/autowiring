// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreObject.h"
#include <list>
#include MEMORY_HEADER

struct AnySharedPointer;
class CoreContext;
class DispatchQueue;

template<class T>
struct JunctionBoxEntry;

/// <summary>
/// Used to identify event managers
/// </summary>
class JunctionBoxBase {
public:
  JunctionBoxBase(void):
    m_isInitiated(false)
  {}
  
  virtual ~JunctionBoxBase(void);

protected:
  // This JunctionBox can fire and receive events
  bool m_isInitiated;

  /// <summary>
  /// Invokes SignalTerminate on each context in the specified vector.  Does not wait.
  /// </summary>
  static void TerminateAll(const std::list<std::weak_ptr<CoreContext>>& teardown);

  /// <summary>
  /// Convenience routine for Fire calls
  /// </summary>
  /// <remarks>
  /// This is a convenience routine, its only purpose is to add the "this" parameter to the
  /// call to FilterFiringException
  /// </remarks>
  void FilterFiringException(const AnySharedPointer& pReceiver) const;

  /// <summary>
  /// Converts a dumb pointer into a weak pointer
  /// </summary>
  /// <remarks>
  /// An exterior hold guarantee must be made to call this method safely
  /// </remarks>
  static std::weak_ptr<CoreContext> ContextDumbToWeak(CoreContext* pContext);

public:
  bool IsInitiated(void) const {return m_isInitiated;}
  void Initiate(void) {m_isInitiated=true;}
  
  /// <summary>
  /// Convenience method allowing consumers to quickly determine whether any listeners exist
  /// </summary>
  /// <returns>
  /// True if at least one listener has been registered
  /// </returns>
  virtual bool HasListeners(void) const = 0;

  // Event attachment and detachment pure virtuals
  virtual void Add(const JunctionBoxEntry<CoreObject>& rhs) = 0;
  virtual void Remove(const JunctionBoxEntry<CoreObject>& rhs) = 0;
};
