#pragma once
#include <boost/chrono/system_clocks.hpp>
#include UNIQUE_PTR_HEADER

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
  DispatchThunkDelayed(boost::chrono::high_resolution_clock::time_point readyAt, std::unique_ptr<DispatchThunkBase> thunk) :
    m_readyAt(readyAt),
    m_thunk(std::move(thunk))
  {}

  DispatchThunkDelayed(DispatchThunkDelayed&& rhs) :
    m_readyAt(rhs.m_readyAt),
    m_thunk(std::move(rhs.m_thunk))
  {}
  
  DispatchThunkDelayed(const DispatchThunkDelayed&)=delete;

private:
  // The time when the thunk becomes ready-to-execute
  boost::chrono::high_resolution_clock::time_point m_readyAt;
  // mutable so this class can be removed from a priority queue.
  mutable std::unique_ptr<DispatchThunkBase> m_thunk;

public:
  // Accessor methods:
  boost::chrono::high_resolution_clock::time_point GetReadyTime(void) const { return m_readyAt; }

  /// <summary>
  /// Extracts the underlying thunk
  /// </summary>
  /// <remarks>
  /// Not really const, but has to be to remove from priority queue.
  /// </remarks>
  std::unique_ptr<DispatchThunkBase> Get(void) const {
    return std::move(m_thunk);
  }

  DispatchThunkDelayed& operator=(DispatchThunkDelayed&& rhs) {
    m_readyAt = rhs.m_readyAt;
    m_thunk = std::move(rhs.m_thunk);
    return *this;
  }
  
  DispatchThunkDelayed& operator=(const DispatchThunkDelayed& rhs)=delete;

  /// <summary>
  /// Operator overload, used to sequence delayed dispatch thunks
  /// </summary>
  bool operator<(const DispatchThunkDelayed& rhs) const {
    return rhs.m_readyAt < m_readyAt;
  }
};