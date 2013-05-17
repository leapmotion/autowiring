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
  boost::mutex m_dispatchLock;
  boost::condition_variable m_queueUpdated;
  std::list<std::function<void ()>> m_dispatchQueue;

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

  /// <summary>
  /// Adds a new method to be dispatched by this queue
  /// </summary>
  void operator+=(std::function<void ()>&& rhs) {
    m_dispatchQueue.push_back(rhs);
  }
};

#endif