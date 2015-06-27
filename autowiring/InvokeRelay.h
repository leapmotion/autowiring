// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "is_any.h"

template<typename T>
class JunctionBox;

/// <summary>
/// Function pointer relay type
/// </summary>
template<class FnPtr>
class InvokeRelay {};

template<class RetType, class T, typename... Args>
class InvokeRelay<RetType (T::*)(Args...)> {
public:
  InvokeRelay(void) = default;

  InvokeRelay(const std::shared_ptr<JunctionBox<T>>& erp, RetType (T::*fnPtr)(Args...)) :
    erp(erp),
    fnPtr(fnPtr)
  {}

  static_assert(!is_any<std::is_rvalue_reference<Args>::value...>::value, "Can't use rvalue references as event argument type");

private:
  std::shared_ptr<JunctionBox<T>> erp;
  RetType (T::*fnPtr)(Args...) = nullptr;

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
InvokeRelay<FnPtr> MakeInvokeRelay(const std::shared_ptr<JunctionBox<T>>& pJunctionBox, FnPtr fnPtr) {
  return InvokeRelay<FnPtr>(pJunctionBox, fnPtr);
}
