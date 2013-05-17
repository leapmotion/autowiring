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
  ~DispatchQueue(void);

private:
  bool m_aborted;
  boost::mutex m_dispatchLock;
  boost::condition_variable m_queueUpdated;
  std::list<std::function<void ()>> m_dispatchQueue;

public:
  /// <summary>
  /// Causes all currently blocked thread members to quit and prevents any blocking
  /// </summary>
  void Abort(void) {
    boost::lock_guard<boost::mutex> lk(m_dispatchLock);
    m_dispatchQueue.clear();
  };

  /// <summary>
  /// Blocks until a new dispatch member is added, dispatches, and then returns
  /// </summary>
  void WaitForEvent(void) {
    boost::unique_lock<boost::mutex> lk(m_dispatchLock);
    if(m_aborted)
      throw dispatch_aborted_exception();

    m_queueUpdated.wait(lk, [this] () {
      return !this->m_dispatchQueue.empty() && !m_aborted;
    });
    m_dispatchQueue.front()();
    m_dispatchQueue.pop_front();
  }

  /// <summary>
  /// Similar to WaitForEvent, but does not block
  /// </summary>
  void DispatchEvent(void) {
    boost::lock_guard<boost::mutex> lk(m_dispatchLock);
    if(m_aborted)
      throw dispatch_aborted_exception();
    if(!m_dispatchQueue.empty()) {
      m_dispatchQueue.front()();
      m_dispatchQueue.pop_front();
    }
  }
};

#endif