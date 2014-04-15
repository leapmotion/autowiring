// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _AUTOWIRABLE_SLOT_H
#define _AUTOWIRABLE_SLOT_H
#include "AutoFactory.h"
#include "autowiring_error.h"
#include FUNCTIONAL_HEADER
#include SHARED_PTR_HEADER
#include <functional>
#include <memory>

class CoreContext;
class GlobalCoreContext;
class Object;

// Utility routine, for users who need a function that does nothing
template<class T>
void NullOp(T) {}

/// <summary>
/// Utility class which represents any kind of autowiring entry that may be deferred to a later date
/// </summary>
class DeferrableAutowiring {
public:
  DeferrableAutowiring(const std::shared_ptr<CoreContext>& context);
  ~DeferrableAutowiring(void) {}

private:
  /// <summary>
  /// This is the context that was available at the time the autowiring was performed.
  /// </summary>
  /// <remarks>
  /// A weak reference is held in hroder to ensure proper teardown, otherwise the
  /// first created member of the context would generate a cyclic reference between
  /// this pointer and the context membership set.
  /// </remarks>
  std::weak_ptr<CoreContext> m_context;

public:
  /// <summary>
  /// Attempts to satisfy this autowiring relationship with the specified candidate object
  /// </summary>
  /// <remarks>
  /// This method returns true when the autowiring is successful.  Once the function returns true,
  /// this method is guaranteed to never be called again.
  /// </remarks>
  virtual bool TrySatisfyAutowiring(const std::shared_ptr<Object>& candidate) = 0;

  /// <summary>
  /// Satisfies autowiring with a so-called "witness type" which is guaranteed to inherit AutowirableSlot
  /// </summary>
  /// <remarks>
  /// The passed value must be statically castable to type AutowirableSlot
  /// </remarks>
  virtual void SatisfyAutowiring(DeferrableAutowiring* pWitness) = 0;

  /// <summary>
  /// Releases memory allocated by this object, where appropriate
  /// </summary>
  /// <summary>
  /// Implementors of this method are permitted to delete "this" or perform any other work while
  /// outside of the context of a lock.  This method is only called after TrySatisfyAutowiring has
  /// returned true.  Once this method returns, this object is guaranteed never to be referred to
  /// again by CoreContext.
  /// </remarks>
  virtual void Finalize(void) {
    // Just reset the enclosing pointer, do nothing else.
    m_context.reset();
  }
};

template<class T>
class AutowirableSlot:
  public std::shared_ptr<T>
{
public:
  typedef T value_type;
  typedef std::shared_ptr<T> t_ptrType;

  operator bool(void) const {
    return IsAutowired();
  }

  virtual bool IsAutowired(void) const = 0;
};

/// <summary>
/// A function-based autowirable slot, which invokes a lambda rather than binding a shared pointer
/// </summary>
template<class Fn, class T>
class AutowirableSlotFn:
  public AutowirableSlot<T>
{
  static_assert(!std::is_same<CoreContext, T>::value, "Do not attempt to autowire CoreContext.  Instead, use AutoCurrentContext or AutoCreateContext");
  static_assert(!std::is_same<GlobalCoreContext, T>::value, "Do not attempt to autowire GlobalCoreContext.  Instead, use AutoGlobalContext");

public:
  AutowirableSlotFn(const std::shared_ptr<CoreContext>& ctxt, Fn&& fn) :
    AutowirableSlot(ctxt),
    fn(std::move(fn))
  {
  }

  // Underlying lambda that we will call:
  Fn fn;

  // Overrides from DeferrableAutowiring:
  bool TrySatisfyAutowiring(const std::shared_ptr<Object>& candidate) override {
    auto casted = std::dynamic_pointer_cast<T>(candidate);
    if(!casted)
      return false;
  }

  void Finalize(void) override {
    // Let the lambda execute as it sees fit:
    fn(casted);

    // Call the lambda, remove all accountability to the context, self-destruct, and return:
    ctxtWeak.reset();
    delete this;
    return true;
  }
};

#endif
