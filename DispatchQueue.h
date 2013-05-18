#ifndef _DISPATCH_QUEUE_H
#define _DISPATCH_QUEUE_H
#include <boost/thread.hpp>
#include <functional>
#include <list>

/// <summary>
/// Thrown when a dispatch operation was aborted
/// </summary>
class dispatch_aborted_exception:
  public std::exception
{
};

/// <summary>
/// A simple virtual class used to hold a trivial thunk
/// </summary>
class DispatchThunkBase {
public:
  virtual ~DispatchThunkBase(void) {}
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
/// This is an asynchronous queue of zero-argument functions
/// </summary>
class DispatchQueue
{
public:
  DispatchQueue(void);

  /// <summary>
  /// Throws an exception if it is invoked, and Abort wasn't first called
  /// </summary>
  /// <remarks>
  /// 
  /// </remarks>
  virtual ~DispatchQueue(void);

private:
  bool m_aborted;

  // A lock held when modifications to any element EXCEPT the first element must be made:
  boost::mutex m_dispatchLock;

  boost::condition_variable m_queueUpdated;

  std::list<std::unique_ptr<DispatchThunkBase>> m_dispatchQueue;

  /// <summary>
  /// Similar to DispatchEvent, except assumes that the dispatch lock is currently held
  /// </summary>
  /// <param name="lk">A lock on m_dispatchLock</param>
  /// <remarks>
  /// This method assumes that the dispatch lock is held and that m_aborted is false.  It
  /// is an error to call this method without those preconditions met.
  /// </remarks>
  void DispatchEventUnsafe(boost::unique_lock<boost::mutex>& lk);

public:
  /// <summary>
  /// Causes all currently blocked thread members to quit and prevents any blocking
  /// </summary>
  void Abort(void);

  /// <summary>
  /// Blocks until a new dispatch member is added, dispatches, and then returns
  /// </summary>
  void WaitForEvent(void);

  /// <summary>
  /// Similar to WaitForEvent, but does not block
  /// </summary>
  void DispatchEvent(void);
  
  template<class _Fx>
  void operator+=(_Fx&& fx) {
    boost::lock_guard<boost::mutex> lk(m_dispatchLock);
    m_dispatchQueue.push_back(
      std::unique_ptr<DispatchThunkBase>(
        new DispatchThunk<_Fx>(fx)
      )
    );
  }
};

#endif