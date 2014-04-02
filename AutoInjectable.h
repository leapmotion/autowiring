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

AutoInjectable operator+(AutoInjectable&& lhs, AutoInjectable&& rhs) {
  AutoInjectable retVal(std::forward<AutoInjectable>(lhs));
  retVal += std::forward<AutoInjectable>(rhs);
  return retVal;
}

AutoInjectable operator+(const AutoInjectable& lhs, const AutoInjectable& rhs) {
  return AutoInjectable(lhs) + AutoInjectable(rhs);
}

AutoInjectable operator+(AutoInjectable&& lhs, const AutoInjectable& rhs) {
  return std::forward<AutoInjectable>(lhs) + AutoInjectable(rhs);
}
AutoInjectable operator+(const AutoInjectable &lhs, AutoInjectable&& rhs) {
  return AutoInjectable(lhs) + rhs;
}

template<class T, class... Args>
AutoInjectable MakeInjectable(Args... args) {
  return AutoInjectable(std::tuple<T*>(), std::forward<Args>(args)...);
}
