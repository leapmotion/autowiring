#pragma once

/// <summary>
/// A simple virtual class used to hold a trivial thunk
/// </summary>
class DispatchThunkBase {
public:
  DispatchThunkBase():
    isCommited(true)
  {}
  
  virtual ~DispatchThunkBase(void){}
  virtual void operator()() = 0;
  
  bool IsCommited() {
    return isCommited;
  }
  
  void Commit(){
    isCommited = true;
  }
  
protected:
  bool isCommited;
};

template<class _Fx>
class DispatchThunk:
  public DispatchThunkBase
{
public:
  DispatchThunk(const _Fx& fx):
    m_fx(fx)
  {
    isCommited = false;
  }

  _Fx m_fx;

  void operator()() override {
    m_fx();
  }
};