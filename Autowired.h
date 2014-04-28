// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "AutowirableSlot.h"
#include "GlobalCoreContext.h"
#include "Decompose.h"
#include FUNCTIONAL_HEADER
#include MEMORY_HEADER
#include RVALUE_HEADER

template<class T>
class Autowired;
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
  AutoCreateContextT(void):
    std::shared_ptr<CoreContext>(CoreContext::CurrentContext()->Create<T>())
  {}
  AutoCreateContextT(std::shared_ptr<CoreContext>& ctxt):
    std::shared_ptr<CoreContext>(ctxt->Create<T>())
  {}
};

typedef AutoCreateContextT<void> AutoCreateContext;


/// <summary>
/// Idiom to enable boltable classes
/// </summary>
template<class T>
class AutoEnable
{
public:
  AutoEnable(void) { CoreContext::CurrentContext()->Enable<T>(); }
};

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

  Autowired(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()) :
    AutowirableSlot<T>(ctxt->template ResolveAnchor<T>()),
    m_pFirstChild(nullptr)
  {
    ctxt->Autowire(*this);
  }

  ~Autowired(void) {
    std::unique_ptr<DeferrableAutowiring> prior;
    for(DeferrableAutowiring* cur = m_pFirstChild; cur; cur = cur->GetFlink())
      prior.reset(cur);
  }

private:
  // The first deferred child known to need registration:
  AutowirableSlot<T>* m_pFirstChild;

public:
  operator T*(void) const {
    return std::shared_ptr<T>::get();
  }

  operator bool(void) const {
    return (AutowirableSlot<T>&) *this;
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
    // We pass a null shared_ptr, because we do not want this slot to attempt any kind of unregistration when
    // it goes out of scope.  Instead, we will manage its entire registration lifecycle, and
    // retain full ownership over the object until we need to destroy it.
    auto newHead = new AutowirableSlotFn<Fn, T>(std::shared_ptr<CoreContext>(), std::forward<Fn>(fn));

    // Append to our list:
    newHead->SetFlink(m_pFirstChild);
    m_pFirstChild = newHead;
  }

  // Base overrides:
  bool TrySatisfyAutowiring(const std::shared_ptr<Object>& slot) override {
    if(*this)
      // Already assigned, this is an error
      throw autowiring_error("Cannot invoke assign on a slot which is already assigned");

    return !!((std::shared_ptr<T>&)*this = AutowirableSlot<T>::m_fast_pointer_cast(slot));
  }

  void Finalize(void) override {
    // Carry the satisfaction to all of our autowirable slots.  If an exception is thrown
    // here, we will allow our destructor to handle cleanup operations.
    while(m_pFirstChild) {
      // Allow the child to obtain a shared_ptr addref:
      m_pFirstChild->SatisfyAutowiring(*this);

      // Need to memoize flink, because Finalize has been defined as a self-destruct routine:
      auto flink = m_pFirstChild->GetFlink();
      m_pFirstChild->Finalize();
      m_pFirstChild = static_cast<AutowirableSlot<T>*>(flink);
    }
  }
};

/// <summary>
/// Similar to Autowired, but doesn't defer creation if types doesn't already exist
/// </summary>
template<class T>
class AutowiredFast:
  public std::shared_ptr<T>
{
public:
  using std::shared_ptr<T>::operator=;

  // !!!!! Read comment in Autowired if you get a compiler error here !!!!!
  AutowiredFast(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()){
    ctxt->FindByTypeRecursive(*this);
  }

  operator bool(void) const {
    return IsAutowired();
  }

  operator T*(void) const {
    return std::shared_ptr<T>::get();
  }

  bool IsAutowired(void) const {return std::shared_ptr<T>::get() != nullptr;}
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

  // !!!!! Read comment in Autowired if you get a compiler error here !!!!!
  AutoRequired(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()):
    std::shared_ptr<T>(ctxt->template Inject<T>())
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
/// Convenience class to create an event firer. Also caches the associated JunctionBox
/// </summary>
template<class T>
  class AutoFired
{
public:
  AutoFired(void):
    m_junctionBox(CoreContext::CurrentContext()->GetJunctionBox<T>())
  {
    static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot AutoFire a non-event type, your type must inherit EventReceiver");
  }

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

  template<class MemFn, bool isDeferred = std::is_same<typename Decompose<MemFn>::retType, Deferred>::value>
  struct Selector {
    typedef std::function<typename Decompose<MemFn>::fnType> retType;

    static inline retType Select(JunctionBox<T>* pJunctionBox, MemFn pfn) {
      return pJunctionBox->Defer(pfn);
    }
  };

  template<class MemFn>
  struct Selector<MemFn, false> {
    typedef std::function<typename Decompose<MemFn>::fnType> retType;

    static inline retType Select(JunctionBox<T>* pJunctionBox, MemFn pfn) {
      return pJunctionBox->Fire(pfn);
    }
  };

public:
  bool HasListeners(void) const {
    //TODO: Refactor this so it isn't messy
    //check: does it have any direct listeners, or are any appropriate marshalling objects wired into the immediate context?
    auto ctxt = CoreContext::CurrentContext();
    bool checkval = ctxt->CheckEventOutputStream<T>();
    return (checkval || (!m_junctionBox.expired() && m_junctionBox.lock()->HasListeners()) );
  }

  template<class MemFn>
  InvokeRelay<MemFn> operator()(MemFn pfn) const {
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot invoke an event for an unrelated type");

    if (m_junctionBox.expired()) return InvokeRelay<MemFn>(); //Context has been destroyed

    return m_junctionBox.lock()->Invoke(pfn);
  }

  template<class MemFn>
  InvokeRelay<MemFn> Fire(MemFn pfn) const {
    static_assert(!std::is_same<typename Decompose<MemFn>::retType, Deferred>::value, "Cannot Fire an event which is marked Deferred");

    return operator()(pfn);
  }

  template<class MemFn>
  InvokeRelay<MemFn> Defer(MemFn pfn) const {
    static_assert(std::is_same<typename Decompose<MemFn>::retType, Deferred>::value, "Cannot Defer an event which does not return the Deferred type");

    return operator()(pfn);
  }
};

// We will also pull in a few utility headers which are reliant upon the declarations in this file
// TODO:  Consider moving the declarations in this file into its own header, and using this header
// as a master header for all of Autowiring
#include "AutoPacketFactory.h"
