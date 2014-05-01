// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include FUNCTIONAL_HEADER

class JunctionBoxBase;
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
///   rethrower(except);
/// } catch(custom_type_1& t1) {
///   ...handling code...
/// } catch(custom_type_2& t2) {
///   ...handling code...
/// }
///
/// Alternatively, this strategy may be used:

/// try {
///   rethrower(except);
/// } catch(custom_type_1&) {
/// } catch(custom_type_2&) {
/// }
/// ...handling code...
///
/// Filtration methods may safely allow any unhandled rethrows to percolate to the caller.
/// Unhandled exceptions thrown in a context will cause that context to be torn down.
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
  /// <param name="rethrower">A function which, when called, will throw the exception being filtered</param>
  /// <remarks>
  /// Such an exception is treated as a fatal exception, and will unconditionally result
  /// in the enclosing context being torn down.  Filters MUST NOT attempt to alter this
  /// behavior.  If this behavior is not desirable, greater care should be taken to
  /// prevent certain exceptions from being unhandled.
  ///
  /// The rethrower lambda must be called before this filter method exits.  Attempting to cache this function
  /// will result in undefined behavior.
  /// </remarks>
  virtual void Filter(const std::function<void()>& rethrower) {};

  /// <summary>
  /// This is an exception filter for exceptions thrown during a call to Fire
  /// </summary>
  /// <param name="rethrower">A function which, when called, will throw the exception being filtered</param>
  /// <param name="pRecipient">The target of the call</param>
  /// <remarks>
  /// The rethrower lambda must be called before this filter method exits.  Attempting to cache this function
  /// will result in undefined behavior.
  /// </remarks>
  virtual void Filter(const std::function<void()>& rethrower, const JunctionBoxBase* pJunctionBox, EventReceiver* pRecipient) {}
};
