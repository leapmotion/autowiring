#pragma once
#include "AutoFuture.h"
#include "CoreContext.h"

class AutoInjectableExpressionBase
{
public:
  virtual ~AutoInjectableExpressionBase(void) {

  }
  virtual void operator()(AutoFuture* pFuture) const = 0;
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

  void operator()(AutoFuture* pFuture) const override {
    auto added = CallByUnpackingTuple(typename gen_seq<sizeof...(Args)>::type());
    if(pFuture)
      *pFuture += added;
  }

private:
  const std::tuple<Args...> m_args;

  template<int... S>
  std::shared_ptr<T> CallByUnpackingTuple(seq<S...>) const {
    auto ctxt = CoreContext::CurrentContext();
    return ctxt->Construct<T>(std::get<S>(m_args)...);
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
  void operator()(AutoFuture*) const override {}
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
    rhs.pFLink = nullptr;
  }

  AutoInjectable(const AutoInjectable &rhs) :
    pValue(rhs.pValue),
    pFLink(rhs.pFLink ? new AutoInjectable(*rhs.pFLink) : nullptr)
  {
  }

  ~AutoInjectable()
  {
    delete pFLink;
    pFLink = nullptr;
  }

  /// <summary>
  /// Primary injection operation, injects this injectable's payload into the current context
  /// </summary>
  /// <param name="pFuture">A future which may be used to assess when injected items complete</param>
  /// <remarks>
  /// This method performs the actual injection proper into the current context, and is idempotent.
  /// Constructor arguments to any injected types are copied, even if the constructor expects an
  /// r-value.  Users are therefore discouraged from using a constructor accepting an R-value with
  /// an Injectable--while this is not technically incorrect, it does imply that a move operation
  /// is taking place when in fact that is not the case.
  /// </remarks>
  void operator()(AutoFuture* pFuture = nullptr) const {
    pValue->operator()(pFuture);
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
  std::shared_ptr<AutoInjectableExpressionBase> pValue;
  AutoInjectable* pFLink;
};

inline AutoInjectable operator+(AutoInjectable lhs, AutoInjectable rhs) {
  return lhs += std::move(rhs);
}

template<class T, class... Args>
AutoInjectable MakeInjectable(Args... args) {
  return AutoInjectable(std::tuple<T*>(), std::forward<Args>(args)...);
}
