// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <vector>
#include "Object.h"
#include MUTEX_HEADER
#include MEMORY_HEADER
#include STL_UNORDERED_SET

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
  // Dispatch queue lock:
  mutable std::mutex m_lock;

  // Just the DispatchQueue listeners:
  typedef std::unordered_set<DispatchQueue*> t_stType;
  t_stType m_dispatch;
  
  // This JunctionBox can fire and receive events
  bool m_isInitiated;

  /// <summary>
  /// Invokes SignalTerminate on each context in the specified vector.  Does not wait.
  /// </summary>
  static void TerminateAll(const std::vector<std::weak_ptr<CoreContext>>& teardown);

  /// <summary>
  /// Convenience routine for Fire calls
  /// </summary>
  /// <remarks>
  /// This is a convenience routine, its only purpose is to add the "this" parameter to the
  /// call to FilterFiringException
  /// </remarks>
  void FilterFiringException(const std::shared_ptr<Object>& pReceiver) const;

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
  
  const std::unordered_set<DispatchQueue*> GetDispatchQueue(void) const { return m_dispatch; }
  std::mutex& GetDispatchQueueLock(void) const { return m_lock; }

  virtual bool HasListeners(void) const = 0;

  // Event attachment and detachment pure virtuals
  virtual void Add(const JunctionBoxEntry<Object>& rhs) = 0;
  virtual void Remove(const JunctionBoxEntry<Object>& rhs) = 0;
};