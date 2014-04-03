#pragma once
#include "CoreContext.h"

class AutoInjectableExpressionBase
{
public:
  virtual ~AutoInjectableExpressionBase(void) {

  }
  virtual void operator()(void) const = 0;

  virtual AutoInjectableExpressionBase* clone() const = 0;
};

/// <summary>
/// An expression type, which generally encapsulates a single injection operation
/// </summary>
template<class T, class... Args>
class AutoInjectableExpression:
  public AutoInjectableExpressionBase
{
public:
  explicit AutoInjectableExpression(Args&&... args) :
    m_args(std::forward<Args>(args)...)
  {}

  void operator()(void) const override {
    CallByUnpackingTuple(typename gen_seq<sizeof...(Args)>::type());
  }

  AutoInjectableExpressionBase* clone() const override {
    return new AutoInjectableExpression<T, Args...>(*this);
  }

private:
  std::tuple<Args...> m_args;

  template<int... S>
  void CallByUnpackingTuple(seq<S...>) const {
    auto ctxt = CoreContext::CurrentContext();
    ctxt->Construct<T>(std::get<S>(m_args)...);
  }
};

/// <summary>
/// A deafult injectable expression with a no-op behavior
/// </summary>
template<>
class AutoInjectableExpression<void> :
  public AutoInjectableExpressionBase
{
public:
  void operator()(void) const override {}
  virtual AutoInjectableExpressionBase* clone() const { return new AutoInjectableExpression(*this); }
};

/// <summary>
/// Represents a composable injectable expression.
/// </summary>
/// <remarks>
/// An injectable is a type of composable factory which allows consumers to describe collections
/// of classes to be injected into a context at the same time, together with any necessary constructor
/// arguments to those classes.  New injectables can be created as the sum of any two injectables.
///
/// Once an AutoInjectable instance has been obtained, the function-call operator may be invoked
/// to create instances of the enclosed payload objects in the current context.  This operation is
/// idempotent according to the ordinary rules of AutoRequire and CoreContext::Construct.
/// </remarks>
class AutoInjectable
{
public:
  template<class T, class... Args>
  AutoInjectable(std::tuple<T*>, Args&&... args) :
    pValue(new AutoInjectableExpression<T, Args...>(std::forward<Args>(args)...)),
    pFLink(nullptr)
  {}

  AutoInjectable(AutoInjectable&& rhs):
    pValue(rhs.pValue),
    pFLink(rhs.pFLink)
  {
    rhs.pValue = nullptr;
    rhs.pFLink = nullptr;
  }

  AutoInjectable(const AutoInjectable &rhs) :
    pValue(rhs.pValue->clone()),
    pFLink(rhs.pFLink ? new AutoInjectable(*rhs.pFLink) : nullptr)
  {
  }

  ~AutoInjectable()
  {
    delete pValue;
    pValue = nullptr;
    delete pFLink;
    pFLink = nullptr;
  }

  /// <summary>
  /// Primary injection operation, injects this injectable's payload into the current context
  /// </summary>
  /// <remarks>
  /// This method performs the actual injection proper into the current context, and is idempotent.
  /// Constructor arguments to any injected types are copied, even if the constructor expects an
  /// r-value.  Users are therefore discouraged from using a constructor accepting an R-value with
  /// an Injectable--while this is not technically incorrect, it does imply that a move operation
  /// is taking place when in fact that is not the case.
  /// </remarks>
  void operator()() const {
    pValue->operator()();
    if(pFLink)
      pFLink->operator()();
  }

  AutoInjectable& operator+=(AutoInjectable&& other) {
    auto pLast = &pFLink;
    while(*pLast)
      pLast = &(*pLast)->pFLink;

    *pLast = new AutoInjectable(std::forward<AutoInjectable>(other));
    return *this;
  }

  void operator=(AutoInjectable&& rhs) {
    std::swap(pValue, rhs.pValue);
    std::swap(pFLink, rhs.pFLink);
  }

private:
  AutoInjectableExpressionBase* pValue;
  AutoInjectable* pFLink;
};

inline AutoInjectable operator+(AutoInjectable lhs, AutoInjectable rhs) {
  return lhs += std::move(rhs);
}

template<class T, class... Args>
AutoInjectable MakeInjectable(Args... args) {
  return AutoInjectable(std::tuple<T*>(), std::forward<Args>(args)...);
}
