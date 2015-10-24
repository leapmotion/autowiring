// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "auto_signal.h"
#include "autowiring_error.h"
#include "CoreContext.h"
#include "fast_pointer_cast.h"
#include "SlotInformation.h"
#include MEMORY_HEADER

struct MemoEntry;
class CoreContext;
class CoreObject;
class DeferrableAutowiring;
class GlobalCoreContext;

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
  DeferrableAutowiring(const DeferrableAutowiring& rhs);
  DeferrableAutowiring(AnySharedPointer&& witness, const std::shared_ptr<CoreContext>& context);
  virtual ~DeferrableAutowiring(void);

protected:
  // Lock for fields on this type
  autowiring::spin_lock m_lock;

  // All registrations attached to this object:
  std::vector<autowiring::registration_t> m_autowired_notifications;

  // The held shared pointer.
  AnySharedPointer m_ptr;

  class Handler {
  public:
    Handler(DeferrableAutowiring& parent, MemoEntry& memo) :
      parent(parent),
      memo(memo)
    {}

    DeferrableAutowiring& parent;
    MemoEntry& memo;

    void operator()();
  };

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
  // Accessor method:
  const AnySharedPointer& GetSharedPointer(void) const { return m_ptr; }
  operator bool(void) const { return m_ptr != nullptr; }

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
  void reset(void);

  /// <summary>
  /// Assigns a lambda function to be called when the dependency for this slot is autowired.
  /// </summary>
  /// <remarks>
  /// In contrast with CoreContext::NotifyWhenAutowired, the specified lambda is only
  /// called as long as this Autowired slot has not been destroyed.  If this slot is destroyed
  /// before the dependency is satisfied, i.e. because the owning context shuts down, the
  /// lambda is cancelled.
  ///
  /// Note that, if T is injected at about the same time as this object is destroyed, then cancellation
  /// of the autowired lambda could potentially race with satisfaction of that same lambda.  This type
  /// of race is called a cancellation race, and can result in the lambda being invoked even though this
  /// object has been destroyed.
  ///
  /// Users who use Autowired as a member of their class do not need to consider this case.  Autowired
  /// fields declared as class members are always cancelled before the enclosing object is destroyed.
  ///
  /// \include snippets/Autowired_Notify.txt
  /// </remarks>
  template<class Fn>
  void NotifyWhenAutowired(Fn&& fn) {
    // Trivial initial check:
    if (*this) {
      fn();
      return;
    }

    if (std::shared_ptr<CoreContext> context = DeferrableAutowiring::m_context.lock()) {
      MemoEntry& entry = context->FindByType(m_ptr.type());

      // Need to memorialize registration of this entry
      auto reg = entry.onSatisfied += std::move(fn);
      if (!reg)
        return;

      std::lock_guard<autowiring::spin_lock> lk(m_lock);
      m_autowired_notifications.push_back(std::move(reg));
    }
  }

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
