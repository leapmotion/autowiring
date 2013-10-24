// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _AUTOWIRED_H
#define _AUTOWIRED_H
#include "AutowirableSlot.h"
#include "CoreContext.h"
#include "AutoFactory.h"
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
  public std::shared_ptr<CoreContext>
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
class AutoCreateContext:
  public std::shared_ptr<CoreContext>
{
public:
  AutoCreateContext(void);
};

template<class T>
class AutowiredCreator:
  public AutowirableSlot,
  public std::shared_ptr<T>
{
public:
  typedef T value_type;
  typedef shared_ptr<T> t_ptrType;
  
  template<class U>
  static typename std::enable_if<has_static_new<U>::value, U*>::type New(void) {
    return U::New();
  }

  template<class U>
  static typename std::enable_if<!has_static_new<U>::value, U*>::type New(void) {
    static_assert(has_simple_constructor<U>::value, "Attempted to create a type which did not provide a zero-arguments ctor");
    return new U;
  }

  /// <summary>
  /// Creates a new instance if this instance isn't autowired
  /// </summary>
  /// <remarks>
  /// If type T has a static member function called New, the helper's Create routine will attempt call
  /// this function instead of the default constructor, even if the default constructor has been supplied,
  /// and even if the arity of the New routine is not zero.
  ///
  /// To prevent this behavior, use a name other than New.
  /// </remarks>
  void Create(void) {
    if(*this)
      return;

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
    this->reset(New<T>());
    AutowirableSlot::LockContext()->Add(*this);
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
  public AutowiredCreator<T>
{
public:
  static_assert(!std::is_same<CoreContext, T>::value, "Do not attempt to autowire CoreContext.  Instead, use AutoCurrentContext or AutoCreateContext");
  static_assert(!std::is_same<GlobalCoreContext, T>::value, "Do not attempt to autowire GlobalCoreContext.  Instead, use AutoGlobalContext");

  Autowired(void) {
    shared_ptr<CoreContext> context = AutowirableSlot::LockContext();
    context->Autowire(*this);
  }
};

/// <summary>
/// Similar to Autowired, but the default constructor invokes Autowired(true)
/// </summary>
/// <remarks>
/// This class is simply a convenience class and provides a declarative way to name a required dependency.
/// </remarks>
template<class T>
class AutoRequired:
  public Autowired<T>
{
public:
  AutoRequired(void) {
    if(!*this)
      AutowiredCreator<T>::Create();
  }
};

/// <summary>
/// Unconditionally creates a new transient member of type T and adds it to the current context
/// </summary>
template<class T>
class AutoTransient:
  public std::shared_ptr<T>
{
public:
  /// <summary>
  /// Constructor which registers the specified transient instance with the passed pool
  /// </summary>
  AutoTransient(TransientPool<T>& pool):
    std::shared_ptr<T>(new T)
  {
    // Associate with the pool:
    pool.Add(*this);
  }
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

    auto ctxt = CoreContext::CurrentContext();
    m_receiver = ctxt->GetEventRecieverProxy<T>();
  }

private:
  template<class MemFn>
  struct Decompose;

  template<class R, class W>
  struct Decompose<R (W::*)()> {
    typedef void fnType();
    typedef W type;
    typedef R retType;
  };

  template<class R, class W, class Arg1>
  struct Decompose<R (W::*)(Arg1)> {
    typedef void fnType(Arg1);
    typedef W type;
    typedef R retType;
  };

  template<class R, class W, class Arg1, class Arg2>
  struct Decompose<R (W::*)(Arg1, Arg2)> {
    typedef void fnType(Arg1, Arg2);
    typedef W type;
    typedef R retType;
  };

  template<class R, class W, class Arg1, class Arg2, class Arg3>
  struct Decompose<R (W::*)(Arg1, Arg2, Arg3)> {
    typedef void fnType(Arg1, Arg2, Arg3);
    typedef W type;
    typedef R retType;
  };

  template<class R, class W, class Arg1, class Arg2, class Arg3, class Arg4>
  struct Decompose<R (W::*)(Arg1, Arg2, Arg3, Arg4)> {
    typedef void fnType(Arg1, Arg2, Arg3, Arg4);
    typedef W type;
    typedef R retType;
  };

  std::shared_ptr<EventReceiverProxy<T>> m_receiver;

  template<class MemFn, bool isDeferred = std::is_same<typename Decompose<MemFn>::retType, Deferred>::value>
  struct Selector {
    typedef std::function<typename Decompose<MemFn>::fnType> retType;

    static inline retType Select(EventReceiverProxy<T>* pReceiver, MemFn pfn) {
      return pReceiver->Defer(pfn);
    }
  };

  template<class MemFn>
  struct Selector<MemFn, false> {
    typedef std::function<typename Decompose<MemFn>::fnType> retType;

    static inline retType Select(EventReceiverProxy<T>* pReceiver, MemFn pfn) {
      return pReceiver->Fire(pfn);
    }
  };

public:
  bool HasListeners(void) const {
    return m_receiver->HasListeners();
  }

  template<class MemFn>
  InvokeRelay<MemFn> operator()(MemFn pfn) const {
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot invoke an event for an unrelated type");
    return m_receiver->Invoke(pfn);
  }

  template<class MemFn>
  InvokeRelay<MemFn> Fire(MemFn pfn) const {
    static_assert(!std::is_same<typename Decompose<MemFn>::retType, Deferred>::value, "Cannot Fire an event which is marked Deferred");
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot Fire an event for an unrelated type");
    return m_receiver->Invoke(pfn);
  }

  template<class MemFn>
  InvokeRelay<MemFn> Defer(MemFn pfn) const {
    static_assert(std::is_same<typename Decompose<MemFn>::retType, Deferred>::value, "Cannot Defer an event which does not return the Deferred type");
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot Defer an event for an unrelated type");
    return m_receiver->Invoke(pfn);
  }
};

#endif
