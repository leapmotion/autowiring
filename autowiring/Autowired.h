// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_id.h"
#include "auto_signal.h"
#include "AutowiringDebug.h"
#include "AutowirableSlot.h"
#include "CoreContext.h"
#include "GlobalCoreContext.h"
#include MEMORY_HEADER
#include ATOMIC_HEADER

/// <summary>
/// Provides a reference to the current context.
/// </summary>
/// <remarks>
/// Do not hold references longer than needed.  Failing
/// to release a context pointer could prevent resources from being released.
///
/// \include snippets/Context_AutoCurrentContext.txt
/// </remarks>
class AutoCurrentContext:
  public std::shared_ptr<CoreContext>
{
public:
  AutoCurrentContext(void);
};

/// <summary>
/// Provides a reference to the global context.
/// </summary>
/// <remarks>
/// One global context is created automatically for an application. In complex
/// applications, take care when using the global context because unexpected interactions
/// can occur between members added to global from different parts of the application.
///
/// \include snippets/Context_AutoGlobalContext.txt
/// </remarks>
class AutoGlobalContext:
  public std::shared_ptr<GlobalCoreContext>
{
public:
  AutoGlobalContext(void);
};

/// <summary>
/// Provides a simple way to create a child of the current context using a sigil.
/// </summary>
/// <remarks>
/// A sigil is a class that serves to classify the context. You can use the sigil to
/// identify a group of related contexts.
///
/// \include snippets/Context_AutoCreateContextT.txt
/// </remarks>
template<typename T>
class AutoCreateContextT:
  public std::shared_ptr<CoreContext>
{
public:
  AutoCreateContextT(void) :
    std::shared_ptr<CoreContext>(CoreContext::CurrentContext()->Create<T>())
  {}

  AutoCreateContextT(const std::shared_ptr<CoreContext>& ctxt) :
    std::shared_ptr<CoreContext>(ctxt->Create<T>())
  {}
};

/// <summary>
/// Creates a new context as a child of the current context.
/// </summary>
/// <remarks>
/// \include snippets/Context_AutoCreateContext.txt
/// </remarks>
typedef AutoCreateContextT<void> AutoCreateContext;


/// <summary>
/// Autowires a "slot" in a context to which an instance of the specified type
/// will be wired when it becomes available.
/// </summary>
/// <param name="T">The class whose type is to be found.  Must be an EXACT match.</param>
/// <remarks>
/// The Autowired template class forms the foundation of the context consumer system.
/// It provides a quick way to import or create an instance of a specified
/// class in the local context.
///
/// \include snippets/Autowired_Autowired.txt
///
/// If an instance of the type specified in the template parameter already exists
/// in the context, the dependency is satisfied using that instance immediately.
/// Otherwise, the dependency is satisfied when an instance of the correct type
/// is created in the context.
///
/// This class may be safely used even when the member in question is an abstract type.
/// </remarks>
template<class T>
class Autowired:
  public AutowirableSlot<T>
{
public:
  Autowired(Autowired&&) = delete;
  Autowired(const Autowired& rhs) :
    AutowirableSlot<T>(static_cast<AutowirableSlot<T>&>(rhs))
  {}

  Autowired(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()) :
    AutowirableSlot<T>(ctxt)
  {}

  operator const std::shared_ptr<T>&(void) const {
    return static_cast<const AnySharedPointerT<T>&>(this->m_ptr).get();
  }
  
  operator std::weak_ptr<T>(void) const {
    return this->operator const std::shared_ptr<T>&();
  }
  
  operator T*(void) const {
    return this->operator const std::shared_ptr<T>&().get();
  }

  template<typename Fn, typename... Args>
  struct wrapper {
    wrapper(const std::weak_ptr<CoreContext>& ctxtWeak, Fn&& fn) :
      ctxtWeak(ctxtWeak),
      fn(std::forward<Fn&&>(fn))
    {}

    std::weak_ptr<CoreContext> ctxtWeak;
    Fn fn;

    void operator()(const Args&... args) {
      if (auto ctxt = ctxtWeak.lock())
        fn(args...);
    }
  };

  template<typename U, typename Fn, typename... Args>
  struct wrapper_delayed {
    wrapper_delayed(Autowired<T>& field, autowiring::signal<void(Args...)> U::*member, Fn&& fn) :
      m_field(field),
      m_wrapper(field.m_context, std::forward<Fn&&>(fn)),
      member(member)
    {}

    Autowired<T>& m_field;
    wrapper<Fn, Args...> m_wrapper;
    autowiring::signal<void(Args...)> U::*member;

    void operator()(void) const {
      auto reg = m_field->*member += std::move(m_wrapper);
      std::lock_guard<autowiring::spin_lock> lk(m_field.m_lock);
      m_field.m_autowired_notifications.push_back(std::move(reg));
    }
  };

  //A small temporary structure for the info required to register an event
  //with a member signal on an autowired field.
  template<typename U, typename... Args>
  struct signal_relay {
    signal_relay(Autowired<T>& f, autowiring::signal<void(Args...)> U::*m) :
      field(&f),
      member(m) 
    {}

    Autowired<T>* field;
    autowiring::signal<void(Args...)> U::*member;

    template<typename Fn>
    void operator+=(Fn&& rhs) {
      field->NotifyWhenAutowired(
        wrapper_delayed<U, Fn, Args...>{ *field, member, std::forward<Fn&&>(rhs) }
      );
    }
  };
  
  template<typename U, typename... Args>
  signal_relay<U,Args...> operator()(autowiring::signal<void(Args...)> U::*sig) {
    static_assert(std::is_base_of<U, T>::value, "Cannot reference member of unrelated type");
    return signal_relay<U, Args... >(*this, sig);
  }

  void operator=(Autowired<T>&& rhs) = delete;
  void operator=(const Autowired<T>& rhs) {
    *this = *static_cast<AutowirableSlot<T>*>(this);
  }
};

/// <summary>
/// Autowires the specified dependency, creating a new instance if one does not already exist.
/// </summary>
/// <remarks>
/// Autowiring searches for a suitable instance to satisfy this dependency in the
/// current context or any of its ancestors. If that search fails, a new instance
/// of the specified type is created.
///
/// \include snippets/Autowired_AutoRequired.txt
///
/// If type T has a static member function called New, Autowiring calls
/// this function instead of the default constructor, even if the default constructor
/// has been supplied, and even if the arity of the New routine is not zero. To
/// prevent this behavior, use a name other than New.
///
/// If you get a compile error when using AutoRequire, ensure that class T is fully
/// defined at the point in the program where the type is autorequired.
/// </remarks>
template<class T>
class AutoRequired:
  public std::shared_ptr<T>
{
public:
  using std::shared_ptr<T>::operator=;

  AutoRequired(const AutoRequired<T>& rhs) :
    std::shared_ptr<T>(rhs)
  {}
  
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
  /// AutoRequired overload that forwards constructor arguments to the type being wired.
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
  
  bool IsAutowired(void) const { return std::shared_ptr<T>::get() != nullptr; }
};

/// <summary>
/// Autowires a slot immediately (with limitations).
/// </summary>
/// <remarks>
/// AutowiredFast provides non-blocking and typically faster services than Autowired.
/// However, an instance satisfying the dependency must already exist. If not, the
/// autowiring slot will never be wired, even if the dependency is created later.
///
/// \include snippets/Autowired_AutowiredFast.txt
///
/// AutowiredFast allows queries to be conducted against contexts that may be in teardown,
/// and also generally operates with fewer memory allocations and better performance than
/// Autowired.  As a drawback, notifications on AutowiredFast cannot be attached, and the
/// field will not be updated in the case of post-hoc satisfaction--the value is effectively
/// a constant after initialization. AutowiredFast also requires that the autowired type
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
  AutowiredFast(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()) :
    AutowiredFast(ctxt.get())
  {}

  AutowiredFast(const CoreContext* pCtxt) {
    (void) autowiring::fast_pointer_cast_initializer<T, CoreObject>::sc_init;
    (void) auto_id_t_init<T>::init;

    const auto& entry = pCtxt->FindByType(auto_id_t<T>{});
    *this = entry.m_value.template as<T>();
  }

  operator bool(void) const {
    return IsAutowired();
  }

  operator T*(void) const {
    return std::shared_ptr<T>::get();
  }

  T* operator->(void) const {
    auto* retVal = std::shared_ptr<T>::operator->();
    if (!retVal)
      throw autowiring_error("Attempted to dereference a null autowired field");
    return retVal;
  }

  T& operator*(void) const {
    T* retVal = std::shared_ptr<T>::get();
    if (!retVal)
      throw autowiring_error("Attempted to dereference a null autowired field");
    return *retVal;
  }

  bool IsAutowired(void) const { return std::shared_ptr<T>::get() != nullptr; }
};

/// <summary>
/// Enables the specified type to be "bolted" to the current context.
/// </summary>
/// <remarks>
/// Used to enable a boltable class in a context and can be used even if the context has not been created yet.  In
/// this case, the class will be constructed and enabled when the context is created
/// </remarks>
template<class T>
class AutoEnable
{
public:
  // !!!!! Read comment in AutoRequired if you get a compiler error here !!!!!
  AutoEnable(std::shared_ptr<CoreContext> ctxt = CoreContext::CurrentContext()) {
    ctxt->Enable<T>();
  }
};

/// \internal
/// <summary>
/// A version of AutoRequired that forwards constructor arguments to the injected
/// type's constructor function.
/// </summary>
template<class T>
class AutoConstruct:
  public std::shared_ptr<T>
{
public:
  template<class... Args>
  AutoConstruct(Args&&... args) :
    std::shared_ptr<T>(init(std::forward<Args&&>(args)...))
  {}

  operator bool(void) const { return IsAutowired(); }
  operator T*(void) const { return std::shared_ptr<T>::get(); }
  bool IsAutowired(void) const { return std::shared_ptr<T>::get() != nullptr; }

private:
  std::shared_ptr<T> init(void) { return CoreContext::CurrentContext()->template Inject<T>(); }
  std::shared_ptr<T> init(const AutoConstruct<T>& rhs) { return rhs; }

  template<typename Arg, typename... Args>
  typename std::enable_if<
    !std::is_same<
      AutoConstruct<T>,
      typename std::decay<Arg>::type
    >::value,
    std::shared_ptr<T>
  >::type init(Arg&& arg, Args&&... args) {
    return CoreContext::CurrentContext()->template Inject<T>(
      std::forward<Arg>(arg),
      std::forward<Args&&>(args)...
    );
  }
};

/// <summary>
/// Injects an object that can fire events.
/// </summary>
/// <remarks>
/// An autowiring event is technically a callback implemented by Autowired members
/// of a context. An event is fired by invoking the event function, which, in turn,
/// invokes the callback functions of all members of the context implementing the
/// function. For example, you might define a simple event class as follows:
///
/// \include snippets/AutoFired_Event.txt
///
/// You can then add an event dispatcher to a context with AutoFired:
///
/// \include snippets/AutoFired_Declare.txt
///
/// and trigger the event with:
///
/// \include snippets/AutoFired_Dispatch.txt
///
/// Now for the event to do anything, you must also autowire members to the context
/// that extend your Event class and implement your Event::eventFunction() (or whatever
/// the class and function are called in your code). For example:
///
/// \include snippets/AutoFired_Receiver.txt
///
/// You can add an instance of class Foo to the context and its eventFunction()
/// will be invoked whenever the event is fired. Note that the context must be
/// “initiated” by calling its Initiate() function before events are propagated.
/// The context code for dispatching and handling this simple event would look like this:
///
/// \include snippets/AutoFired_Example.txt
///
/// Which will print the following line to the standard output stream:
///
/// \include snippets/AutoFired_Result.txt
///
/// If other members of the context inherited Event — or contained their own
/// autowired members that inherited Event, the callback functions implemented by
/// those members would also be invoked.
/// </remarks>
template<class T>
  class AutoFired
{
public:
  AutoFired(const std::shared_ptr<CoreContext>& ctxt = CoreContext::CurrentContext()):
    m_junctionBox(ctxt->GetJunctionBox<T>())
  {}

  /// \internal
  /// <summary>
  /// Utility constructor, used when the receiver is already known
  /// </summary>
  AutoFired(const std::shared_ptr<JunctionBox<T>>& junctionBox) :
    m_junctionBox(junctionBox)
  {}

  /// \internal
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
    return
      box ?
      MakeInvokeRelay(box, pfn) :
      InvokeRelay<MemFn>();
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
