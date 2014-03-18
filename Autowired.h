// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _AUTOWIRED_H
#define _AUTOWIRED_H
#include "AutowirableSlot.h"
#include "CreationRules.h"
#include "GlobalCoreContext.h"
#include "Decompose.h"
#include "TypeRegistry.h"
#include <functional>
#include <memory>

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
  public AutowirableSlot,
  public std::shared_ptr<T>
{
  static_assert(!std::is_same<CoreContext, T>::value, "Do not attempt to autowire CoreContext.  Instead, use AutoCurrentContext or AutoCreateContext");
  static_assert(!std::is_same<GlobalCoreContext, T>::value, "Do not attempt to autowire GlobalCoreContext.  Instead, use AutoGlobalContext");

public:
  typedef T value_type;
  typedef shared_ptr<T> t_ptrType;

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

  Autowired(void):
    AutowirableSlot(CoreContext::CurrentContext() -> template ResolveAnchor<T>())
  {
    AutowirableSlot::LockContext() -> Autowire(*this);
  }

  Autowired(std::weak_ptr<CoreContext> ctxt):
    AutowirableSlot(ctxt.lock() -> template ResolveAnchor<T>())
  {
    AutowirableSlot::LockContext() -> Autowire(*this);
  }

  operator bool(void) const {
    return IsAutowired();
  }

  operator T*(void) const {
    return t_ptrType::get();
  }

  bool IsAutowired(void) const override {return !!t_ptrType::get();}
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
  public AutowirableSlot,
  public std::shared_ptr<T>
{
  static_assert(!std::is_same<CoreContext, T>::value, "Do not attempt to autowire CoreContext.  Instead, use AutoCurrentContext or AutoCreateContext");
  static_assert(!std::is_same<GlobalCoreContext, T>::value, "Do not attempt to autowire GlobalCoreContext.  Instead, use AutoGlobalContext");
public:
  using std::shared_ptr<T>::operator=;
  typedef T value_type;
  typedef shared_ptr<T> t_ptrType;

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

  AutoRequired(void):
    AutowirableSlot(CoreContext::CurrentContext() -> template ResolveAnchor<T>())
  {
    *this = AutowirableSlot::LockContext()->template Inject<T>();
  }

  AutoRequired(std::weak_ptr<CoreContext> ctxt):
    AutowirableSlot(ctxt.lock() -> template ResolveAnchor<T>())
  {
    *this = AutowirableSlot::LockContext()->template Inject<T>();
  }

  operator bool(void) const {
    return IsAutowired();
  }

  operator T*(void) const {
    return t_ptrType::get();
  }

  bool IsAutowired(void) const override {return !!t_ptrType::get();}
};


/// <summary>
/// This class
/// </summary>
template<class T>
  class AutoFired
{
public:
  AutoFired(void) {
    static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot AutoFire a non-event type, your type must inherit EventReceiver");

    // Add an utterance of the TypeRegistry so we can add this AutoFired type to our collection
    (void)RegType<T>::r;

    auto ctxt = CoreContext::CurrentContext();
    m_junctionBox = ctxt->GetJunctionBox<T>();
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

#endif
