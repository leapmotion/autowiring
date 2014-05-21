#pragma once
#include "fast_pointer_cast.h"
#include "SharedPointerSlot.h"
#include "SlotInformation.h"
#include MEMORY_HEADER

class CoreContext;
class DeferrableAutowiring;
class GlobalCoreContext;
class Object;

// Utility routine, for users who need a function that does nothing
template<class T>
void NullOp(T) {}

/// <summary>
/// Strategy class for performing unsynchronized operations on an autowirable slot
/// </summary>
/// <remarks>
/// The DeferrableAutowiring base class' SatisfyAutowiring routine is  guaranteed to be run in a
/// synchronized context--IE, a call to CancelAutowiringNotification will block until the above
/// routines return.  Unfortunately, this lock also excludes many other types of operations, such
/// as type search operations, which means that some of the work associated with cleaning up after
/// an autowiring has been satisfied will take place in an unsynchronized context.   This means
/// that virtual function calls are generally unsafe on the member being autowired when they are
/// made without a lock being held.
///
/// To mitigate this problem, instead of performing a virtual call through the original object, a
/// strategy type is provided by the DeferrableAutowiring while the lock is held, and then later the
/// strategy is employed to clean up the object, if necessary.
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
  virtual void Finalize(DeferrableAutowiring* pSlot) const = 0;
};

/// <summary>
/// Utility class which represents any kind of autowiring entry that may be deferred to a later date
/// </summary>
class DeferrableAutowiring {
public:
  DeferrableAutowiring(const std::shared_ptr<CoreContext>& context);
  virtual ~DeferrableAutowiring(void);

protected:
  /// <summary>
  /// This is the context that was available at the time the autowiring was performed.
  /// </summary>
  /// <remarks>
  /// A weak reference is held in hroder to ensure proper teardown, otherwise the
  /// first created member of the context would generate a cyclic reference between
  /// this pointer and the context membership set.
  /// </remarks>
  std::weak_ptr<CoreContext> m_context;

  // The next entry to be autowired in this sequence
  DeferrableAutowiring* m_pFlink;

protected:
  /// <summary>
  /// Causes this deferrable to unregister itself with the enclosing context
  /// </summary>
  void CancelAutowiring(void);

public:
  // Accessor methods:
  DeferrableAutowiring* GetFlink(void) { return m_pFlink; }

  // Mutator methods:
  void SetFlink(DeferrableAutowiring* pFlink) {
    m_pFlink = pFlink;
  }

  /// <returns>
  /// The type on which this deferred slot is bound
  /// </returns>
  virtual const std::type_info& GetType(void) const = 0;

  /// <returns>
  /// The strategy that should be used to satisfy this slot
  /// </returns>
  /// <remarks>
  /// If no custom strategy is required, this method may return null
  /// </remarks>
  virtual const DeferrableUnsynchronizedStrategy* GetStrategy(void) { return nullptr; }

  /// <summary>
  /// </summary>
  virtual DeferrableAutowiring* ReleaseDependentChain(void) { return nullptr; }

  /// <summary>
  /// Satisfies autowiring with a so-called "witness slot" which is guaranteed to be satisfied on the same type
  /// </summary>
  /// <remarks>
  /// The passed value must be a void pointer exactly to a shared_ptr of type T that matches this slot
  /// </remarks>
  virtual void SatisfyAutowiring(const void* pvSharedPtr) = 0;
};

template<class T>
class AutowirableSlot:
  public DeferrableAutowiring,
  public std::shared_ptr<T>
{
public:
  typedef T value_type;
  typedef std::shared_ptr<T> t_ptrType;

  AutowirableSlot(const std::shared_ptr<CoreContext>& ctxt) :
    DeferrableAutowiring(ctxt),
    m_type(typeid(T)),
    m_fast_pointer_cast(&leap::fast_pointer_cast<T, Object>)
  {
    SlotInformationStackLocation::RegisterSlot(this);
  }

  virtual ~AutowirableSlot(void) {
    CancelAutowiring();
  }

  const std::type_info& m_type;
  std::shared_ptr<T>(*const m_fast_pointer_cast)(const std::shared_ptr<Object>&);

  // Must be final, because we use this in our dtor and need its behavior to be fixed:
  const std::type_info& GetType(void) const override {
    return m_type;
  }

  void SatisfyAutowiring(const void* pvSharedPtr) override {
    // Cast over and assign:
    (std::shared_ptr<T>&)*this = *(const std::shared_ptr<T>*)pvSharedPtr;
  }

  operator bool(void) const {
    return IsAutowired();
  }

  bool IsAutowired(void) const { return !!t_ptrType::get(); }

  using std::shared_ptr<T>::operator=;
};

/// <summary>
/// A function-based autowirable slot, which invokes a lambda rather than binding a shared pointer
/// </summary>
template<class T, class Fn>
class AutowirableSlotFn:
  public AutowirableSlot<T>
{
  static_assert(!std::is_same<CoreContext, T>::value, "Do not attempt to autowire CoreContext.  Instead, use AutoCurrentContext or AutoCreateContext");
  static_assert(!std::is_same<GlobalCoreContext, T>::value, "Do not attempt to autowire GlobalCoreContext.  Instead, use AutoGlobalContext");

  class Strategy:
    public DeferrableUnsynchronizedStrategy
  {
  public:
    Strategy(void) {}

    void Finalize(DeferrableAutowiring* pfn) const override {
      ((AutowirableSlotFn*) pfn)->Finalize();
    }
  };

  static const Strategy s_strategy;

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
  void Finalize(void) {
    // Let the lambda execute as it sees fit:
    CallThroughObj<Fn>(&Fn::operator());

    // Call the lambda, remove all accountability to the context, self-destruct, and return:
    this->m_context.reset();
    delete this;
  }

  template<class L, class Ret, class... Args>
  void CallThroughObj(Ret(L::*pfn)(Args...) const) {
    (fn.*pfn)(
      (Args) *this...
    );
  }

  const DeferrableUnsynchronizedStrategy* GetStrategy(void) override { return &s_strategy; }
};

template<class T, class Fn>
AutowirableSlotFn<T, Fn>* MakeAutowirableSlotFn(const std::shared_ptr<CoreContext>& ctxt, Fn fn) {
  return new AutowirableSlotFn<T, Fn>(ctxt, std::forward<Fn>(fn));
}

template<class T, class Fn>
const typename AutowirableSlotFn<T, Fn>::Strategy AutowirableSlotFn<T, Fn>::s_strategy;
