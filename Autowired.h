// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _AUTOWIRED_H
#define _AUTOWIRED_H
#include "AutowirableSlot.h"
#include "CoreContext.h"
#include <functional>
#include <memory>

template<class T>
class Autowired;
class CoreContext;
class GlobalCoreContext;

// Redeclarations, primary declarations in CoreContext.h
std::shared_ptr<GlobalCoreContext> GetGlobalContext(void);

template<class T>
class AutowiredCreator:
  public AutowirableSlot,
  public std::shared_ptr<T>
{
public:
  typedef shared_ptr<T> t_ptrType;

  /// <summary>
  /// Creates a new instance if this instance isn't autowired
  /// </summary>
  /// <remarks>
  /// Users are encouraged to make use of AutoRequired wherever it's a sensible alternative
  /// to a Create call.
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
    this->reset(new T);
    AutowirableSlot::LockContext()->Add(*this);
  }

  /// <summary>
  /// This creates a pointer using the specified lambda, if such creation is needed.
  /// <summary>
  void Create(const std::function<T* ()>& fn) {
    // Is the object already created?  Short-circuit if so.
    if(*this)
      return;

    // Okay, we're ready to go now, we can release
    // the shared pointer so any lambdas disappear
    AutowirableSlot::m_tracker = std::shared_ptr<AutowirableSlot>();

    // TODO:  Allow this to be lazily invoked
    // It would be nice if this constructor is only invoked on the first dereference
    // of this autowired object.  That would allow us to specify default types that
    // aren't constructed spuriously.
    T* ptr = fn();

    // Now we'll add this object to the context so the created object may be autowired elsewhere.
    // We also want to be sure we use the same shared_ptr that's being used internally in the
    // context.
    std::shared_ptr<CoreContext> context = LockContext();
    *this = context->Add(ptr);
  }

  operator bool(void) const {
    return IsAutowired();
  }

  operator T*(void) const {
    return t_ptrType::get();
  }

  AutowiredCreator<T>& operator=(T* rhs) {
    // Set up the shared pointer first:
    std::shared_ptr<T>::reset(rhs);

    // Only add when we are non-null
    if(rhs)
      // Strong assumption must be made, here, that the rhs isn't already in the current context
      LockContext()->Add(*this);
    return *this;
  }

  bool IsAutowired(void) const override {return !!t_ptrType::get();}
};

/// <summary>
/// This is the specialization for global contexts.  Unlike other autowires, it's guaranteed
/// to autowire in all circumstances.
/// </summary>
/// <remarks>
/// We do not autowire operator=, because there is never a case where the rhs is anything
/// but the sole Global context or null.
/// </remarks>
template<>
class AutowiredCreator<GlobalCoreContext>:
  public AutowirableSlot,
  public std::shared_ptr<GlobalCoreContext>
{
private:
  // We do not allow operator=
  using std::shared_ptr<GlobalCoreContext>::operator=;

public:
  typedef shared_ptr<GlobalCoreContext> t_ptrType;

  AutowiredCreator(void):
    std::shared_ptr<GlobalCoreContext>(GetGlobalContext())
  {
  }

  bool IsAutowired(void) const override {return !!t_ptrType::get();}

  operator GlobalCoreContext*(void) const {
    return t_ptrType::get();
  }
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
  Autowired(void) {
    shared_ptr<CoreContext> context = AutowirableSlot::LockContext();
    context->Autowire(*this);
  }

  Autowired(T* ptr) {
    *this = ptr;
  }

  using AutowiredCreator<T>::operator=;
};

/// <summary>
/// This is a forbidden autowiring.  Do not attempt it.  Instead, use AutoCurrentContext or AutoCreateContext.
/// </summary>
template<>
class Autowired<CoreContext>
{
private:
  Autowired(void);
};

template<>
class Autowired<GlobalCoreContext>:
  public AutowiredCreator<GlobalCoreContext>
{};

/// <summary>
/// Similar to Autowired, but the default constructor invokes Autowired(true)
/// </summary>
/// <remarks>
/// This class is simply a convenience class and provides a declarative way to name a
/// required dependency.
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
/// A special templated type that allows users to specify factory construction methods
/// </summary>
template<class T, T* (*fn)()>
struct CtorProxy {};

/// <summary>
/// An AutoRequired specialization that allows the user to specify a function call to initialize this field
/// </summary>
/// <remarks>
/// This specialization is useful when it's necessary to AutoRequire an interface
/// </remarks>
template<class T, T* (*fn)()>
class AutoRequired<CtorProxy<T, fn>>:
  public Autowired<T>
{
public:
  AutoRequired(void) {
    if(*this)
      return;

    this->reset(fn());
    AutowirableSlot::LockContext()->Add(*this);
  }
};

/// <summary>
/// A special templated type that allows users to specify a particular concrete instance
/// </summary>
template<class T, class Concrete>
struct CtorConcrete {};

template<class T, class Concrete>
class AutoRequired<CtorConcrete<T, Concrete>>:
  public Autowired<T>
{
public:
  AutoRequired(void) {
    if(*this)
      return;

    this->reset(new Concrete);
    AutowirableSlot::LockContext()->Add(*this);
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
/// An AutowiredLocal instance may only be satisfied by a member of the specified type which exists in the current context.
/// </summary>
/// <remarks>
/// Do not use AutoRequiredLocal and AutoRequired on the same type in the same context.  Doing this could cause an initialization-order
/// dependency, and is an error.  In debug mode, doing this may cause an exception.
/// </remarks>
template<class T>
class AutowiredLocal:
  public AutowiredCreator<T>
{
public:
  AutowiredLocal(void) {
    shared_ptr<CoreContext> context = AutowirableSlot::LockContext();
    context->Autowire(*this);
  }
};

/// <summary>
/// A local AutoRequired instance will ensure that the specified type will always be constructed in the current scope
/// </summary>
/// <remarks>
/// This type offers a convenient way to ensure that some type is always constructed in the current context, even if a satisfying
/// type exists in the parent scope.  Do not use AutoRequiredLocal and AutoRequired on the same type in the same context.  Doing this
/// could cause an initialization-order dependency, and is an error.  In debug mode, doing this may cause an exception.
/// </remarks>
template<class T>
class AutoRequiredLocal:
  public AutowiredLocal<T>
{
public:
  AutoRequiredLocal(void) {
    if(!*this)
      AutowiredCreator<T>::Create();
  }
};

/// <summary>
/// This class
/// </summary>
template<class T>
class AutoFired
{
public:
  static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot AutoFire a non-event type, your type must inherit EventReceiver");

  AutoFired(void) {
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
  typename Selector<MemFn>::retType operator()(MemFn pfn) const {
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot invoke an event for an unrelated type");
    return Selector<MemFn>::Select(m_receiver.get(), pfn);
  }

  template<class MemFn>
  std::function<typename Decompose<MemFn>::fnType> Fire(MemFn pfn) const {
    static_assert(!std::is_same<typename Decompose<MemFn>::retType, Deferred>::value, "Cannot Fire an event which is marked Deferred");
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot Fire an event for an unrelated type");
    return m_receiver->Fire(pfn);
  }

  template<class MemFn>
  std::function<typename Decompose<MemFn>::fnType> Defer(MemFn pfn) const {
    static_assert(std::is_same<typename Decompose<MemFn>::retType, Deferred>::value, "Cannot Defer an event which does not return the Deferred type");
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot Defer an event for an unrelated type");
    return m_receiver->Defer(pfn);
  }
};

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

  using std::shared_ptr<CoreContext>::operator=;
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

  using std::shared_ptr<CoreContext>::operator=;
};

#endif
