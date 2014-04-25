#pragma once
#include "autowiring_error.h"
#include "fast_pointer_cast.h"
#include <functional>
#include FUNCTIONAL_HEADER
#include MEMORY_HEADER
#include RVALUE_HEADER

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
  /// <remarks>
  /// In order to ensure that GetType is available,
  /// </remarks>
  void CancelAutowiring(void);

public:
  DeferrableAutowiring* GetFlink(void) const { return m_pFlink; }
  void SetFlink(DeferrableAutowiring* pFlink) { m_pFlink = pFlink; }


  /// <returns>
  /// The type on which this deferred slot is bound
  /// </returns>
  virtual const std::type_info& GetType(void) const = 0;

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
  virtual void SatisfyAutowiring(const DeferrableAutowiring& witness) = 0;

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
  {}

  virtual ~AutowirableSlot(void) {
    CancelAutowiring();
  }

  const std::type_info& m_type;
  std::shared_ptr<T>(*const m_fast_pointer_cast)(const std::shared_ptr<Object>&);

  // Must be final, because we use this in our dtor and need its behavior to be fixed:
  const std::type_info& GetType(void) const override {
    return m_type;
  }

  bool TrySatisfyAutowiring(const std::shared_ptr<Object>& candidate) override {
    *this = m_fast_pointer_cast(candidate);
    return *this;
  }

  void SatisfyAutowiring(const DeferrableAutowiring& witness) override {
    // Just perform blind assignment:
    SatisfyAutowiring(static_cast<const AutowirableSlot<T>&>(witness));
  }

  void SatisfyAutowiring(const AutowirableSlot<T>& witness) {
    // Just perform blind assignment:
    (std::shared_ptr<T>&)*this = witness;
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
template<class Fn, class T>
class AutowirableSlotFn:
  public AutowirableSlot<T>
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

  template<class L, class Ret, class... Args>
  void CallThroughObj(Ret(L::*pfn)(Args...) const) {
    (fn.*pfn)(
      (Args) *this...
    );
  }

  void Finalize(void) override {
    // Let the lambda execute as it sees fit:
    CallThroughObj<Fn>(&Fn::operator());

    // Call the lambda, remove all accountability to the context, self-destruct, and return:
    DeferrableAutowiring::m_context.reset();
    delete this;
  }
};

