// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutowirableSlot.h"
#include "Decompose.h"
#include "GlobalCoreContext.h"
#include MEMORY_HEADER
#include ATOMIC_HEADER

class CoreContext;
class GlobalCoreContext;

/// <summary>
/// Provides a simple way to obtain a reference to the current context
/// </summary>
/// <remarks>
/// Users of this class are encouraged not to hold references for longer than needed.  Failing
/// to release a context pointer could prevent resources from being correctly released.
/// </remarks>
class AutoCurrentContext:
  public std::shared_ptr<CoreContext>
{
public:
  AutoCurrentContext(void);
};

/// <summary>
/// Simple way to obtain a reference to the global context
/// </summary>
class AutoGlobalContext:
  public std::shared_ptr<GlobalCoreContext>
{
public:
  AutoGlobalContext(void);
};

/// <summary>
/// Provides a simple way to create a dependent context pointer
/// </summary>
/// <remarks>
/// The newly created context will be created using CoreContext::CurrentContext()->Create().
/// </remarks>
template<typename T>
class AutoCreateContextT:
  public std::shared_ptr<CoreContext>
{
public:
  AutoCreateContextT(void) :
    std::shared_ptr<CoreContext>(CoreContext::CurrentContext()->Create<T>())
  {}

  AutoCreateContextT(AutoInjectable&& inj) :
    std::shared_ptr<CoreContext>(CoreContext::CurrentContext()->Create<T>(std::move(inj)))
  {}

  AutoCreateContextT(std::shared_ptr<CoreContext>& ctxt) :
    std::shared_ptr<CoreContext>(ctxt->Create<T>())
  {}

  AutoCreateContextT(std::shared_ptr<CoreContext>& ctxt, AutoInjectable&& inj) :
    std::shared_ptr<CoreContext>(ctxt->Create<T>(std::move(inj)))
  {}
};

typedef AutoCreateContextT<void> AutoCreateContext;


/// <summary>
/// An autowired template class that forms the foundation of the context consumer system
/// </summary>
/// <param name="T">The class whose type is to be found.  Must be an EXACT match.</param>
/// <remarks>
/// The autowired class offers a quick way to import or create an instance of a specified
/// class in the local context.
///
/// This class may be safely used even when the member in question is an abstract type.
/// </remarks>
template<class T>
class Autowired:
  public AutowirableSlot<T>
{
public:
  Autowired(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()) :
    AutowirableSlot<T>(ctxt),
    m_pFirstChild(nullptr)
  {
    if(ctxt)
      ctxt->Autowire(
      *static_cast<AnySharedPointerT<T>*>(
        static_cast<AnySharedPointer*>(this)
      ),
      *this
    );
  }

  ~Autowired(void) {
    if(m_pFirstChild == this)
      // Tombstoned, nothing to do:
      return;

    // Need to ensure that nobody tries to autowire us while we are tearing down:
    this->CancelAutowiring();

    // And now we destroy our deferrable autowiring collection:
    std::unique_ptr<DeferrableAutowiring> prior;
    for(DeferrableAutowiring* cur = m_pFirstChild; cur; cur = cur->GetFlink())
      prior.reset(cur);
  }

private:
  // The first deferred child known to need registration.  This is distinct from the m_pFlink entry in
  // the base class, which refers to the _next sibling_; by contrast, this type refers to the _first child_
  // which will be the first member registered via NotifyWhenAutowired.
  std::atomic<AutowirableSlot<T>*> m_pFirstChild;

public:
  operator const std::shared_ptr<T>&(void) const {
    return
      static_cast<const AnySharedPointerT<T>*>(
        static_cast<const AnySharedPointer*>(
          this
        )
      )->slot()->get();
  }
  
  operator std::weak_ptr<T>(void) const {
    return this->operator const std::shared_ptr<T>&();
  }
  
  operator T*(void) const {
    return this->operator const std::shared_ptr<T>&().get();
  }
  
  /// <summary>
  /// Allows a lambda function to be called when this slot is autowired
  /// </summary>
  /// <remarks>
  /// In contrast with CoreContext::NotifyWhenAutowired, the specified lambda will only be
  /// called as long as this Autowired slot has not been destroyed.  If this slot is destroyed
  /// beforehand, the lambda will never be invoked.
  /// </remarks>
  template<class Fn>
  void NotifyWhenAutowired(Fn fn) {
    // Trivial initial check:
    if(*this) {
      fn();
      return;
    }

    // We pass a null shared_ptr, because we do not want this slot to attempt any kind of unregistration when
    // it goes out of scope.  Instead, we will manage its entire registration lifecycle, and
    // retain full ownership over the object until we need to destroy it.
    auto newHead = new AutowirableSlotFn<T, Fn>(std::shared_ptr<CoreContext>(), std::forward<Fn>(fn));

    // Append to our list in a lock-free way.  This is a fairly standard way to do a lock-free append to
    // a singly linked list; the only unusual aspect is the use of "this" as a tombstone indicator (IE,
    // to signify that the list should no longer be used).
    AutowirableSlot<T>* pFirstChild;
    do {
      // Obtain the current first child, and keep it here so we know what to exchange out with later:
      pFirstChild = m_pFirstChild;

      // Determine if we've been tombstoned at this piont:
      if(pFirstChild == this) {
        // Trivially satisfy, and then return.  This might look like a leak, but it's not, because we know
        // that Finalize is going to destroy the object.
        newHead->SatisfyAutowiring(*this);
        newHead->Finalize();
        return;
      }

      // Try to set the forward link to the current head, and then update our own flink;
      newHead->SetFlink(pFirstChild);
    } while(!m_pFirstChild.compare_exchange_weak(pFirstChild, newHead, std::memory_order_acquire));
  }

  // Base overrides:
  DeferrableAutowiring* ReleaseDependentChain(void) override {
    // Rip the head off the list, replace it with a tombstone:
    auto pFirstChild = m_pFirstChild.exchange(this, std::memory_order_relaxed);

    // If we got the tombstone back, we have nothing to return.  Otherwise return the element.
    return pFirstChild == this ? nullptr : pFirstChild;
  }
};

/// <summary>
/// Similar to Autowired, Creates a new instance if this instance isn't autowired
/// </summary>
/// <remarks>
/// This class is simply a convenience class and provides a declarative way to name a required dependency.
///
/// If type T has a static member function called New, the helper's Create routine will attempt call
/// this function instead of the default constructor, even if the default constructor has been supplied,
/// and even if the arity of the New routine is not zero.
///
/// To prevent this behavior, use a name other than New.
/// </remarks>
template<class T>
class AutoRequired:
  public std::shared_ptr<T>
{
public:
  using std::shared_ptr<T>::operator=;
  
  // !!!!! READ THIS IF YOU ARE GETTING A COMPILER ERROR HERE !!!!!
  // If you are getting an error tracked to this line, ensure that class T is totally
  // defined at the point where the Autowired instance is constructed.  Generally,
  // such errors are tracked to missing header files.  A common mistake, for instance,
  // is to do something like this:
  //
  // class MyClass;
  //
  // struct MyStructure {
  //   Autowired<MyClass> m_member;
  // };
  //
  // At the time m_member is instantiated, MyClass is an incomplete type.  So, when the
  // compiler tries to instantiate AutowiredCreator::Create (the function you're in right
  // now!) it finds that it can't create a new instance of type MyClass because it has
  // no idea how to construct it!
  //
  // This problem can be fixed two ways:  You can include the definition of MyClass before
  // MyStructure is defined, OR, you can give MyStructure a nontrivial constructor, and
  // then ensure that the definition of MyClass is available before the nontrivial
  // constructor is defined.
  //
  // !!!!! READ THIS IF YOU ARE GETTING A COMPILER ERROR HERE !!!!!
  AutoRequired(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()):
    std::shared_ptr<T>(ctxt->template Inject<T>())
  {}
  
  /// <summary>
  /// Construct overload, for types which take constructor arguments
  /// </summary>
  template<class... Args>
  AutoRequired(const std::shared_ptr<CoreContext>& ctxt, Args&&... args) :
    std::shared_ptr<T>(ctxt->template Inject<T>(std::forward<Args>(args)...))
  {}
  
  operator bool(void) const {
    return IsAutowired();
  }
  
  operator T*(void) const {
    return std::shared_ptr<T>::get();
  }
  
  bool IsAutowired(void) const {return std::shared_ptr<T>::get() != nullptr;}
};

/// <summary>
/// Provides restricted, non-blocking, and typically faster services than Autowired
/// </summary>
/// <remarks>
/// AutowiredFast allows queries to be conducted against contexts that may be in teardown,
/// and also generally operates with fewer memory allocations and better performance than
/// Autowired.  As a drawback, notifications on AutowiredFast cannot be attached, and the
/// field will not be updated in the case of post-hoc satisfaction--the value is effectively
/// a constant after initialization.  AutowiredFast also requires that the autowired type
/// be completely defined before construction.  By comparison, Autowired fields do not ever
/// need to be defined.
/// </remarks>
template<class T>
class AutowiredFast:
  public std::shared_ptr<T>
{
public:
  using std::shared_ptr<T>::operator=;

  // !!!!! Read comment in AutoRequired if you get a compiler error here !!!!!
  AutowiredFast(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()) {
    (void) autowiring::fast_pointer_cast_initializer<T, Object>::sc_init;

    if (ctxt)
      ctxt->FindByTypeRecursive(*this);
  }

  AutowiredFast(const CoreContext* pCtxt) {
    (void) autowiring::fast_pointer_cast_initializer<T, Object>::sc_init;

    pCtxt->FindByTypeRecursive(*this);
  }

  operator bool(void) const {
    return IsAutowired();
  }

  operator T*(void) const {
    return std::shared_ptr<T>::get();
  }

  bool IsAutowired(void) const { return std::shared_ptr<T>::get() != nullptr; }
};

/// <summary>
/// Idiom to enable boltable classes
/// </summary>
template<class T>
class AutoEnable
{
public:
  // !!!!! Read comment in AutoRequired if you get a compiler error here !!!!!
  AutoEnable(std::shared_ptr<CoreContext> ctxt = CoreContext::CurrentContext()) {
    ctxt->Enable<T>();
  }
};

/// <summary>
/// Convenience class which attempts to inject type T and discards any exceptions
/// </summary>
/// <remarks>
/// Use of this type is functionally equivalent to the following:
///
///  try {
///    AutoCurrentContext()->Inject&lt;T&gt;();
///  catch(...) {
///    AutoCurrentContext()->FilterException();
///  }
///  Autowired&lt;T&gt; foo;
///
/// Users who wish to know whether an exception was thrown may replace uses of AutoDesired with the above
/// and perform their own handling.
/// </remarks>
template<class T>
class AutoDesired:
  public Autowired<T>
{
public:
  AutoDesired(void) {
    try { AutoRequired<T>(); }
    catch(...) {
      CoreContext::CurrentContext()->FilterException();
    }
  }
};

/// <summary>
/// Convenience class functionally identical to AutoRequired, but specialized to forward ctor arguments
/// </summary>
template<class T>
class AutoConstruct:
  public std::shared_ptr<T>
{
public:
  template<class... Args>
  AutoConstruct(Args&&... args) :
    std::shared_ptr<T>(CoreContext::CurrentContext()->template Inject<T>(std::forward<Args&&>(args)...))
  {}

  operator bool(void) const { return IsAutowired(); }
  operator T*(void) const { return std::shared_ptr<T>::get(); }
  bool IsAutowired(void) const { return std::shared_ptr<T>::get() != nullptr; }
};

/// <summary>
/// Convenience class to create an event firer. Also caches the associated JunctionBox
/// </summary>
template<class T>
  class AutoFired
{
public:
  AutoFired(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()):
    m_junctionBox(ctxt->GetJunctionBox<T>())
  {}

  /// <summary>
  /// Utility constructor, used when the receiver is already known
  /// </summary>
  AutoFired(const std::shared_ptr<JunctionBox<T>>& junctionBox) :
    m_junctionBox(junctionBox)
  {}

  /// <summary>
  /// Utility constructor, used to support movement operations
  /// </summary>
  AutoFired(AutoFired&& rhs):
    m_junctionBox(std::move(rhs.m_junctionBox))
  {}

private:
  std::weak_ptr<JunctionBox<T>> m_junctionBox;

public:
  bool HasListeners(void) const {
    //TODO: Refactor this so it isn't messy
    //check: does it have any direct listeners, or are any appropriate marshalling objects wired into the immediate context?
    auto junctionBox = m_junctionBox.lock();
    return junctionBox && junctionBox->HasListeners();
  }

  template<class MemFn>
  InvokeRelay<MemFn> operator()(MemFn pfn) const {
    auto box = m_junctionBox.lock();
    if(!box)
      // Context has been destroyed
      return InvokeRelay<MemFn>();

    AutoGlobalContext()->Invoke(&AutowiringEvents::EventFired)(*CoreContext::CurrentContext(),typeid(typename Decompose<MemFn>::type));

    return MakeInvokeRelay(box, pfn);
  }

  template<class MemFn>
  InvokeRelay<MemFn> Fire(MemFn pfn) const {
    return operator()(pfn);
  }
};

// We will also pull in a few utility headers which are reliant upon the declarations in this file
// TODO:  Consider moving the declarations in this file into its own header, and using this header
// as a master header for all of Autowiring
#include "AutoPacketFactory.h"
