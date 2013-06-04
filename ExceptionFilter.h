#ifndef _EXCEPTION_FILTER_H
#define _EXCEPTION_FILTER_H
#include <exception>

class CoreThread;

/// <summary>
/// Implements an exception filter type, invoked when an unhandled exception is thrown
/// </summary>
class ExceptionFilter
{
public:
  /// <summary>
  /// This method is invoked when an exception has been thrown by CoreThread::Run
  /// </summary>
  virtual void Filter(std::exception_ptr except) const = 0;
  
  /// <summary>
  /// This is an exception thrown during a call to Fire
  /// </summary>
  /// <remarks>
  /// 
  virtual void Filter(std::exception_ptr except, EventReceiver* pRecipient) const = 0;
};

#endif