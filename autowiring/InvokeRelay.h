// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "is_any.h"
#include "index_tuple.h"
#include "JunctionBox.h"

class Deferred;

template<typename T>
class JunctionBox;

/// <summary>
/// A fully bound member function call
/// </summary>
template<class T, class... Args>
class CurriedInvokeRelay:
  public DispatchThunkBase
{
public:
  CurriedInvokeRelay(CurriedInvokeRelay& rhs) = delete;
  CurriedInvokeRelay(CurriedInvokeRelay&& rhs) = delete;

  CurriedInvokeRelay(T& obj, Deferred(T::*fnPtr)(Args...), Args... args) :
    m_obj(obj),
    m_fnPtr(fnPtr),
    m_args(std::forward<Args>(args)...)
  {}

private:
  // The object on which we are bound
  T& m_obj;

  // Function call to be made, and its arguments:
  Deferred(T::*m_fnPtr)(Args...);
  std::tuple<typename std::decay<Args>::type...> m_args;

  /// <summary>
  /// Places a call to the bound member function pointer by unpacking a lambda into it
  /// </summary>
  template<int... S>
  void CallByUnpackingTuple(index_tuple<S...>) {
    (m_obj.*m_fnPtr)(std::move(std::get<S>(m_args))...);
  }

public:
  void operator()(void) override {
    CallByUnpackingTuple(typename make_index_tuple<sizeof...(Args)>::type());
  }
};

/// <summary>
/// Function pointer relay type
/// </summary>
template<class FnPtr>
class InvokeRelay {};

template<typename T, typename ...Args>
class InvokeRelay<Deferred (T::*)(Args...)> {
public:
  InvokeRelay(std::shared_ptr<JunctionBox<T>> erp, Deferred (T::*fnPtr)(Args...)):
    erp(erp),
    fnPtr(fnPtr)
  {}

  // Null constructor
  InvokeRelay():
    erp(nullptr)
  {}

  static_assert(!is_any<std::is_rvalue_reference<Args>...>::value, "Can't use rvalue references as event argument type");

private:
  std::shared_ptr<JunctionBox<T>> erp;
  Deferred (T::*fnPtr)(Args...);

public:
  void operator()(const typename std::decay<Args>::type&... args) const {
    if(!erp)
      // Context has already been destroyed
      return;
    
    if(!erp->IsInitiated())
      // Context not yet started
      return;

    std::lock_guard<std::mutex> lk(erp->GetDispatchQueueLock());
    for(DispatchQueue* q : erp->GetDispatchQueue())
      q->AddExisting(new CurriedInvokeRelay<T, Args...>(dynamic_cast<T&>(*q), fnPtr, args...));
  }
};

template<class T, typename... Args>
class InvokeRelay<void (T::*)(Args...)> {
public:
  InvokeRelay(std::shared_ptr<JunctionBox<T>> erp, void (T::*fnPtr)(Args...)) :
    erp(erp),
    fnPtr(fnPtr)
  {}

  // Null constructor
  InvokeRelay():
    erp(nullptr)
  {}

  static_assert(!is_any<std::is_rvalue_reference<Args>...>::value, "Can't use rvalue references as event argument type");

private:
  std::shared_ptr<JunctionBox<T>> erp;
  void (T::*fnPtr)(Args...);

public:
  /// <summary>
  /// The function call operation itself
  /// </summary>
  /// <returns>False if an exception was thrown by a recipient, true otherwise</returns>
  bool operator()(Args... args) const {
    if(!erp)
      // Context has already been destroyed
      return true;
    
    if(!erp->IsInitiated())
      // Context not yet started
      return true;

    // Give the serializer a chance to handle these arguments:
    erp->SerializeInit(fnPtr, args...);

    auto fw = [this](T& obj, Args... args) {
      (obj.*fnPtr)(args...);
    };

    return erp->FireCurried(
      std::move(fw),
      args...
    );
  }
};

/// <summary>
/// Makes an invocation relay for a particular junction box and function pointer
/// </summary>
template<typename T, typename FnPtr>
InvokeRelay<FnPtr> MakeInvokeRelay(std::shared_ptr<JunctionBox<T>> pJunctionBox, FnPtr fnPtr) {
  return InvokeRelay<FnPtr>(pJunctionBox, fnPtr);
}
