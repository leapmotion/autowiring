#ifndef _EVENT_DISPATCHER_H
#define _EVENT_DISPATCHER_H

/// <summary>
/// This is an interface which generally marks any object that might be able to dispatch events
/// </summary>
class EventDispatcher {
public:
  /// <summary>
  /// Required service method which indicates whether this dispatcher can accept new input
  /// </summary>
  /// <remarks>
  /// Not all implementors who are
  /// </summary>
  virtual bool CanAccept(void) const = 0;

  /// <summary>
  /// Blocks until a new dispatch member is added, dispatches, and then returns
  /// </summary>
  virtual void WaitForEvent(void) = 0;

  /// <summary>
  /// Similar to WaitForEvent, but does not block
  /// </summary>
  virtual void DispatchEvent(void) = 0;
};

#endif