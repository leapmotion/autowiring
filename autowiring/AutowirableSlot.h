// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "auto_signal.h"
#include "autowiring_error.h"
#include "fast_pointer_cast.h"
#include "SlotInformation.h"
#include MEMORY_HEADER

class CoreContext;
class DeferrableAutowiring;
class GlobalCoreContext;
class CoreObject;

/// <summary>
/// Strategy class for performing unsynchronized operations on an autowirable slot
/// </summary>
/// <remarks>
/// The DeferrableAutowiring base class' SatisfyAutowiring routine is  guaranteed to be run in a
/// synchronized context.  Unfortunately, this lock also excludes many other types of operations, such
/// as CoreContext::Inject and CoreContext::FindByType, which means that handing control to a user
/// specified callback is unsafe.
///
/// Exacerbating the problem is the fact that the original DeferrableAutowiring may refer to an
/// object on the stack or whose destruction cannot otherwise be delayed.  As soon as the synchronized
/// context is exited, the object could already be in a teardown pathway, which means we can't invoke
/// any kind of virtual function call on the object.
///
/// Thus, the Finalize operation is only supported on objects whose lifetimes can be externally
/// guaranteed.  Currently, only AutowirableSlotFn supports this behavior, and it is accessable via
/// CoreContext::NotifyWhenAutowired and Autowired::NotifyWhenAutowired.
/// </remarks>
class DeferrableUnsynchronizedStrategy {
public:
  ~DeferrableUnsynchronizedStrategy(void) {}

  /// <summary>
  /// Releases memory allocated by this object, where appropriate
  /// </summary>
  /// <summary>
  /// Implementors of this method are permitted to delete "this" or perform any other work while
  /// outside of the context of a lock.  Once this method returns, this object is guaranteed never
  /// to be referred to again by CoreContext.
  /// </remarks>
  virtual void Finalize(void) = 0;
};

/// <summary>
/// Utility class which represents any kind of autowiring entry that may be deferred to a later date
/// </summary>
class DeferrableAutowiring
{
public:
  DeferrableAutowiring(AnySharedPointer&& witness, const std::shared_ptr<CoreContext>& context);
  virtual ~DeferrableAutowiring(void);

protected:
  // The held shared pointer
  AnySharedPointer m_ptr;

  /// <summary>
  /// This is the context that was available at the time the autowiring was performed.
  /// </summary>
  /// <remarks>
  /// A weak reference is held in hroder to ensure proper teardown, otherwise the
  /// first created member of the context would generate a cyclic reference between
  /// this pointer and the context membership set.
  /// </remarks>
  std::weak_ptr<CoreContext> m_context;

  /// The unique ptr to the registration of the singal handler which is used to finish
  /// the autowiring. The registration can be used to cancel the signal handler.
  std::unique_ptr<autowiring::registration_t> m_deferred_registration;

  /// Cancel the signal handler regsitered in m_deferred_regsitration. This is used when this
  /// is already autowired or to cancel autowiring.
  void UnregisterDeferredAutowire(void);

  /// <summary>
  /// Causes this deferrable to unregister itself with the enclosing context
  /// </summary>
  void CancelAutowiring(void);

public:
  // Accessor method:
  const AnySharedPointer& GetSharedPointer(void) const { return m_ptr; }

  /// <returns>
  /// True if the underlying field is autowired
  /// </returns>
  bool IsAutowired(void) const { return !!m_ptr.ptr(); }

  /// <returns>
  /// The type on which this deferred slot is bound
  /// </returns>
  auto_id GetType(void) const {
    return m_ptr.type();
  }
  
  /// <summary>
  /// Performs a full reset of this slot
  /// </summary>
  /// <remarks>
  /// This operation prevents the slot from being satisfied, if it hasn't already been satisfied, and causes
  /// the pointed-to object to be set to null.  Any attached dependant chains are also destroyed.
  ///
  /// This method may not be safely called from an unsynchronized context.  Callers must ensure that
  /// this field is not in use during the call to reset or a data race will result.
  /// </remarks>
  virtual void reset(void);

  /// <returns>
  /// The strategy that should be used to satisfy this slot
  /// </returns>
  /// <remarks>
  /// If no custom strategy is required, this method may return null
  /// </remarks>
  virtual DeferrableUnsynchronizedStrategy* GetStrategy(void) { return nullptr; }

  /// <summary>
  /// Satisfies autowiring with a so-called "witness slot" which is guaranteed to be satisfied on the same type
  /// </summary>
  void SatisfyAutowiring(const AnySharedPointer& ptr);

  /// <summary>
  /// Register the singal handler which is used to finish the autowiring
  /// </summary>
  void RegisterDeferredAutowire(autowiring::registration_t&& reg);

  bool operator!=(const AnySharedPointer& rhs) const { return m_ptr != rhs; }
  bool operator==(const AnySharedPointer& rhs) const { return m_ptr == rhs; }

  template<typename U>
  bool operator==(const std::shared_ptr<U>& rhs) const {
    return m_ptr == rhs;
  }
};

template<class T>
class AutowirableSlot:
  public DeferrableAutowiring
{
public:
  typedef T value_type;

  AutowirableSlot(const std::shared_ptr<CoreContext>& ctxt) :
    DeferrableAutowiring(AnySharedPointerT<typename std::remove_const<T>::type>(), ctxt)
  {
    SlotInformationStackLocation::RegisterSlot(this);
  }

  virtual ~AutowirableSlot(void) {
    CancelAutowiring();
  }

  /// <remarks>
  /// Shadowing GetType call, provided here as a virtual function optimization
  /// </remarks>
  const std::type_info& GetType(void) const {
    return typeid(T);
  }

  bool IsAutowired(void) const {
    // If the user wishes to know if this type is instantiated, we will require that a full definition
    // of this type MUST be available.  The reason for this is that, if the user wishes to know if a
    // type is autowired, they are required at a minimum to know what that type's inheritance relations
    // are to other types in the system.
    (void) autowiring::fast_pointer_cast_initializer<T, CoreObject>::sc_init;
    (void) auto_id_t_init<T>::init;
    return DeferrableAutowiring::IsAutowired();
  }

  /// <remarks>
  /// Obtains a pointer to the underlying type, if autowired
  /// </remarks>
  T* get(void) const {
    // For now, we require that the full type be available to use this method
    (void) autowiring::fast_pointer_cast_initializer<T, CoreObject>::sc_init;
    (void) auto_id_t_init<T>::init;
    return get_unsafe();
  }

  /// <remarks>
  /// Obtains a pointer to the underlying type, if autowired
  /// </remarks>
  /// <remarks>
  /// Users are STRONGLY discouraged from making use of this routine.  This function does not cause
  /// any runtime type information about T to wind up in any of Autowiring's type registries, and
  /// this may prevent the type from ever being detected as autowirable as a result.
  /// </remarks>
  T* get_unsafe(void) const {
    return static_cast<T*>(m_ptr.ptr());
  }

  explicit operator bool(void) const {
    return IsAutowired();
  }

  T* operator->(void) const {
    // Initialize any blind fast casts to the actually desired type.  This is one of a few points
    // where we can guarantee that the type will be completely defined, because the user is about
    // to make use of this type.
    (void) autowiring::fast_pointer_cast_initializer<T, CoreObject>::sc_init;
    (void) auto_id_t_init<T>::init;

    auto retVal = get();
    if (!retVal)
      throw autowiring_error("Attempted to dereference a null autowired field");
    return retVal;
  }

  T& operator*(void) const {
    auto retVal = get();
    if (!retVal)
      throw autowiring_error("Attempted to dereference a null autowired field");

    // We have to initialize here, in the operator context, because we don't actually know if the
    // user will be making use of this type.
    (void) autowiring::fast_pointer_cast_initializer<T, CoreObject>::sc_init;
    (void) auto_id_t_init<T>::init;

    return *retVal;
  }
};

template<typename T>
bool operator==(const std::shared_ptr<T>& lhs, const AutowirableSlot<T>& rhs) {
  return rhs == lhs;
}

/// <summary>
/// A function-based autowirable slot, which invokes a lambda rather than binding a shared pointer
/// </summary>
template<class T, class Fn>
class AutowirableSlotFn:
  public AutowirableSlot<T>,
  public DeferrableUnsynchronizedStrategy
{
  static_assert(!std::is_same<CoreContext, T>::value, "Do not attempt to autowire CoreContext.  Instead, use AutoCurrentContext or AutoCreateContext");
  static_assert(!std::is_same<GlobalCoreContext, T>::value, "Do not attempt to autowire GlobalCoreContext.  Instead, use AutoGlobalContext");

public:
  AutowirableSlotFn(const std::shared_ptr<CoreContext>& ctxt, Fn&& fn) :
    AutowirableSlot<T>(ctxt),
    fn(std::move(fn))
  {
  }

  virtual ~AutowirableSlotFn(void) {
    DeferrableAutowiring::CancelAutowiring();
  }

  // Underlying lambda that we will call:
  const Fn fn;

  /// <summary>
  /// Finalization routine, called by our strategy
  /// </summary>
  void Finalize(void) override {
    // Let the lambda execute as it sees fit:
    CallThroughObj(fn, &Fn::operator());

    // Call the lambda, remove all accountability to the context, self-destruct, and return:
    this->m_context.reset();
    delete this;
  }

  template<class L, class Ret, class... Args>
  void CallThroughObj(const Fn& fn, Ret(L::*pfn)(Args...) const) {
    (fn.*pfn)(
      (Args) *this...
    );
  }

  DeferrableUnsynchronizedStrategy* GetStrategy(void) override { return this; }
};

