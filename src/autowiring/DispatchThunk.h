// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include CHRONO_HEADER
#include <memory>

namespace autowiring {

/// <summary>
/// A simple virtual class used to hold a trivial thunk
/// </summary>
class DispatchThunkBase {
public:
  virtual ~DispatchThunkBase(void){}
  virtual void operator()() = 0;

  DispatchThunkBase* m_pFlink = nullptr;
};

template<class _Fx>
class DispatchThunk:
  public DispatchThunkBase
{
public:
  DispatchThunk(_Fx&& fx) :
    m_fx(std::forward<_Fx&&>(fx))
  {}

  _Fx m_fx;

  void operator()() override {
    m_fx();
  }
};

template<typename Fx>
std::unique_ptr<DispatchThunkBase> MakeDispatchThunk(Fx&& fx) {
  return std::unique_ptr<DispatchThunkBase>(new DispatchThunk<Fx>(std::forward<Fx&&>(fx)));
}

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
  DispatchThunkDelayed(std::chrono::steady_clock::time_point readyAt, DispatchThunkBase* thunk) :
    m_readyAt(readyAt),
    m_thunk(thunk)
  {}

  DispatchThunkDelayed(DispatchThunkDelayed&& rhs) :
    m_readyAt(rhs.m_readyAt),
    m_thunk(std::move(rhs.m_thunk))
  {}

private:
  // The time when the thunk becomes ready-to-execute
  std::chrono::steady_clock::time_point m_readyAt;
  mutable std::unique_ptr<DispatchThunkBase> m_thunk;

public:
  // Accessor methods:
  std::chrono::steady_clock::time_point GetReadyTime(void) const { return m_readyAt; }
  std::unique_ptr<DispatchThunkBase>& GetThunk(void) const { return m_thunk; }

  /// <summary>
  /// Operator overload, used to sequence delayed dispatch thunks
  /// </summary>
  bool operator<(const DispatchThunkDelayed& rhs) const {
    return rhs.m_readyAt < m_readyAt;
  }

  void operator=(DispatchThunkDelayed&& rhs) {
    m_readyAt = rhs.m_readyAt;
    m_thunk = std::move(rhs.m_thunk);
  }
  void operator=(const DispatchThunkDelayed&) const = delete;
};

}
