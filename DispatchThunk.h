#pragma once
#include <boost/chrono/system_clocks.hpp>

/// <summary>
/// A simple virtual class used to hold a trivial thunk
/// </summary>
class DispatchThunkBase {
public:
  virtual ~DispatchThunkBase(void){}
  virtual void operator()() = 0;
};

template<class _Fx>
class DispatchThunk:
  public DispatchThunkBase
{
public:
  DispatchThunk(const _Fx& fx):
    m_fx(fx)
  {}

  _Fx m_fx;

  void operator()() override {
    m_fx();
  }
};

/// <summary>
/// A so-called "delayed" dispatch thunk which must not be executed prior to the specified time
/// </summary>
class DispatchThunkDelayed {
public:
  /// <summary>
  /// Creates a new delayed dispatch thunk which will be ready at the specified time, and is based on the specified thunk
  /// </summary>
  /// <remarks>
  /// When this dispatch thunk becomes ready, the associated thunk will be pushed to the back to the owning dispatch queue's
  /// ready queue.
  /// </remarks>
  DispatchThunkDelayed(boost::chrono::high_resolution_clock::time_point readyAt, DispatchThunkBase* thunk) :
    m_readyAt(readyAt),
    m_thunk(thunk)
  {}

  DispatchThunkDelayed(DispatchThunkDelayed&& rhs) :
    m_readyAt(rhs.m_readyAt),
    m_thunk(rhs.m_thunk)
  {
    rhs.m_thunk = nullptr;
  }
  
  DispatchThunkDelayed(const DispatchThunkDelayed& rhs) :
    m_readyAt(rhs.m_readyAt),
    m_thunk(rhs.m_thunk)
  {}

  // Little bit of a hack to support non-C++11
  void Reset(void) {
    if(m_thunk)
      delete m_thunk;
  }

private:
  // The time when the thunk becomes ready-to-execute
  boost::chrono::high_resolution_clock::time_point m_readyAt;
  DispatchThunkBase* m_thunk;

public:
  // Accessor methods:
  boost::chrono::high_resolution_clock::time_point GetReadyTime(void) const { return m_readyAt; }

  /// <summary>
  /// Extracts the underlying thunk
  /// </summary>
  DispatchThunkBase* Get(void) const {
    return m_thunk;
  }

  DispatchThunkDelayed& operator=(DispatchThunkDelayed&& rhs) {
    m_readyAt = rhs.m_readyAt;
    std::swap(m_thunk, rhs.m_thunk);
    return *this;
  }
  
  DispatchThunkDelayed& operator=(const DispatchThunkDelayed& rhs) {
    m_readyAt = rhs.m_readyAt;
    m_thunk = rhs.m_thunk;
    return *this;
  }

  /// <summary>
  /// Operator overload, used to sequence delayed dispatch thunks
  /// </summary>
  bool operator<(const DispatchThunkDelayed& rhs) const {
    return rhs.m_readyAt < m_readyAt;
  }
};
