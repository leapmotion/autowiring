#pragma once
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

/// <summary>
/// A simple virtual class used to hold a trivial thunk
/// </summary>
class DispatchThunkBase {
public:
  DispatchThunkBase(bool p_commitNow):
    m_isCommited(p_commitNow)
  {}
  
  virtual ~DispatchThunkBase(void){}
  virtual void operator()() = 0;
  
  bool IsCommited() {
    return m_isCommited;
  }
  
  void Commit(){
    m_isCommited = true;
  }
  
protected:
  bool m_isCommited;
};

template<class _Fx>
class DispatchThunk:
  public DispatchThunkBase
{
public:
  DispatchThunk(const _Fx& fx, bool p_commitNow=false):
    DispatchThunkBase(p_commitNow),
    m_fx(fx)
  {}

  _Fx m_fx;

  void operator()() override {
    m_fx();
  }
};