// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef _EVENT_DISPATCHER_H
#define _EVENT_DISPATCHER_H
/*
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

protected:
  /// <summary>
  /// Blocks the caller until the dispatcher is able to accept a dispatch
  /// </summary>
  /// <returns>True on success, false if the delay was interrupted</returns>
  /// <remarks>
  /// This method will return when the dispatch queue begins accepting events.  It may also
  /// return when it determines that it is impossible for the dispatch queue to ever accept
  /// events--this may happen if the dispatch thread quits, for example.
  /// </remarks>
  virtual bool DelayUntilCanAccept(void) = 0;

  /// <summary>
  /// Blocks until a new dispatch member is added, dispatches, and then returns
  /// </summary>
  virtual void WaitForEvent(void) = 0;

  /// <summary>
  /// Similar to WaitForEvent, but does not block
  /// </summary>
  virtual bool DispatchEvent(void) = 0;

  /// <summary>
  /// Identical to while(DispatchEvent());
  /// </summary>
  /// <returns>The number of events dispatched</returns>
  virtual int DispatchAllEvents(void) = 0;
};
 */

#endif
