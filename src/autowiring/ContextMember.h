// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreObject.h"
#include "TeardownNotifier.h"
#include MEMORY_HEADER

class CoreContext;

/// <summary>
/// A class that must be inherited in order to be a member of a context heriarchy
/// </summary>
class ContextMember:
  public CoreObject,
  public autowiring::TeardownNotifier,
  public std::enable_shared_from_this<ContextMember>
{
protected:
  ContextMember(const char* name = nullptr);

public:
  virtual ~ContextMember();

protected:
  // Pointer back to the enclosing context
  const std::weak_ptr<CoreContext> m_context;

  // Name of this Contextmember:
  const char* m_name;

public:
  // Accessor methods:
  const char* GetName(void) const {return m_name;}
  bool IsOrphaned(void) const {return m_context.expired();}

  /// <summary>
  /// Invoked by the parent context when the parent context is about to be destroyed
  /// </summary>
  /// <remarks>
  /// A context may be destroyed if and only if none of its members are running and none of
  /// them may enter a runnable state.  This happens when the last pointer to ContextMember
  /// is lost.  Resource cleanup must be started at this point.  Context members are deemed
  /// to be unable to enter a running state if they were not signalled to enter this state
  /// before the last shared pointer to their outer CoreContext is released.
  ///
  /// For contexts containing strictly heirarchial objects, implementors of this method do
  /// not need to do anything.  If, however, there are circular references anywhere in the
  /// context, callers should invoke reset() on each autowired member they control.
  ///
  /// TODO:  Eventually, we MUST eliminate this method and simply keep track of all Autowired
  /// instances in a single context.  This can be done safely by using the Autowired's internal
  /// m_tracker member, which can serve to notify listeners when the instance is destroyed.
  /// Alternatively, the Autowired instance could attach and detach itself from a linked list
  /// in a lock-free way in order to support chain detachment.
  /// </remarks>
  virtual void NotifyContextTeardown(void) {}

  /// <summary>
  /// Retrieves the context associated with this object.
  /// </summary>
  /// <remarks>
  /// By default, the context will be whatever the current context was in the thread
  /// where this object was constructed at the time of the object's construction.
  ///
  /// Note that, if the context is in the process of tearing down, this return value
  /// could be null.
  /// </remarks>
  std::shared_ptr<CoreContext> GetContext(void) const { return m_context.lock(); }

  /// <summary>
  /// Returns a shared pointer that refers to ourselves
  /// </summary>
  template<class T>
  std::shared_ptr<const T> GetSelf(void) const {
    return std::static_pointer_cast<const T, const ContextMember>(shared_from_this());
  }

  /// <summary>
  /// Returns a shared pointer that refers to ourselves
  /// </summary>
  template<class T>
  std::shared_ptr<T> GetSelf(void) {
    return std::static_pointer_cast<T, ContextMember>(shared_from_this());
  }
};

