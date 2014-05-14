#pragma once
#include "is_any.h"

class Deferred;

template<typename T>
class JunctionBox;

/// <summary>
/// Function pointer relay type
/// </summary>
template<class FnPtr>
class InvokeRelay {};

template<typename T, typename ...Args>
class InvokeRelay<Deferred (T::*)(Args...)> {
public:
  InvokeRelay(const JunctionBox<T>* erp, Deferred (T::*fnPtr)(Args...)):
    erp(erp),
    fnPtr(fnPtr)
  {}

  // Null constructor
  InvokeRelay():
    erp(nullptr)
  {}

  static_assert(!is_any<std::is_rvalue_reference<Args>...>::value, "Can't use rvalue references as event argument type");

private:
  const JunctionBox<T>* erp;
  Deferred (T::*fnPtr)(Args...);

public:
  void operator()(const typename std::decay<Args>::type&... args) const {
    if(!erp)
      // Context has already been destroyed
      return;
    
    if(!erp->IsInitiated())
      // Context not yet started
      return;

    const auto& dq = erp->GetDispatchQueue();
    boost::lock_guard<boost::mutex> lk(erp->GetDispatchQueueLock());

    for(auto q = dq.begin(); q != dq.end(); q++)
        (**q).AddExisting(new CurriedInvokeRelay<T, Args...>(dynamic_cast<T&>(**q), fnPtr, args...));
  }
};

template<class T, typename... Args>
class InvokeRelay<void (T::*)(Args...)> {
public:
  InvokeRelay(JunctionBox<T>* erp, void (T::*fnPtr)(Args...)) :
    erp(erp),
    fnPtr(fnPtr)
  {}

  // Null constructor
  InvokeRelay():
    erp(nullptr)
  {}

  static_assert(!is_any<std::is_rvalue_reference<Args>...>::value, "Can't use rvalue references as event argument type");

private:
  JunctionBox<T>* erp;
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
