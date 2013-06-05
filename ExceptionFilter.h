#ifndef _EXCEPTION_FILTER_H
#define _EXCEPTION_FILTER_H
#include <exception>

#ifdef __APPLE__
#include "exception_ptr.h"
#endif

class EventManagerBase;
class EventReceiver;

/// <summary>
/// Implements an exception filter type, invoked when an unhandled exception is thrown
/// </summary>
/// <remarks>
/// An exception filter receives an instance of type exception_ptr, and is invoked in
/// a catch handler from the appropriate site.
///
/// Implementors of this interface must use std::rethrow_exception in order to filter
/// the passed exception.  Generally, the filtration technique should be written as
/// follows:
///
/// try {
///   rethrow_exception(except);
/// } catch(custom_type_1&) {
///   ...handling code...
/// } catch(custom_type_2&) {
///   ...handling code...
/// }
///
/// Filtration methods should generally allow any unhandled rethrows to percolate to the
/// caller.  If a filter method does not rethrow the passed exception, the exception is
/// marked as "handled," which may change system behavior under certain circumstances.
/// Generally speaking, unhandled exceptions will result in the teardown of the context
/// that threw them, and by extension, any associated child contexts.
///
/// All filter methods are guaranteed to be called in the thread and context of the code
/// which threw the exception.  In the case of Fire, it will be in the context of the
/// caller.  In the case of Defer, it will be in the context of the callee.
/// </remarks>
class ExceptionFilter
{
public:
  virtual ~ExceptionFilter(void) {}

  /// <summary>
  /// This method is invoked when an exception has been thrown by CoreThread::Run
  /// </summary>
  /// <remarks>
  /// Such an exception is treated as a fatal exception, and will unconditionally result
  /// in the enclosing context being torn down.  Filters MUST NOT attempt to alter this
  /// behavior.  If this behavior is not desirable, greater care should be taken to
  /// prevent certain exceptions from being unhandled.
  /// </remarks>
  virtual void Filter(std::exception_ptr except) {};
  
  /// <summary>
  /// This is an exception thrown during a call to Fire
  /// </summary>
  /// <param name="except">The exception that was thrown</param>
  /// <param name="pRecipient">The target of the call</param>
  virtual void Filter(std::exception_ptr& except, const EventManagerBase* pSender, EventReceiver* pRecipient) {}
};

#endif