// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once

class JunctionBoxBase;
class CoreObject;

/// <summary>
/// Implements an exception filter type, invoked when an unhandled exception is thrown
/// </summary>
/// <remarks>
/// An exception filter is called with an exception current in the application, and it
/// can access this exception either with current_exception or via throw.
///
/// Implementors of this interface must use throw or std::rethrow_exception in order to
/// filter the passed exception.  Generally, the filtration technique should be written
/// as follows:
///
/// \code
/// try {
///   throw;
/// } catch(custom_type_1& t1) {
///   ...handling code...
/// } catch(custom_type_2& t2) {
///   ...handling code...
/// }
/// \endcode
///
/// Alternatively, this strategy may be used:
///
/// \code
/// try {
///   throw;
/// } catch(custom_type_1&) {
/// } catch(custom_type_2&) {
/// }
/// ...handling code...
/// \endcode
///
/// Filtration methods may safely allow any unhandled rethrows to percolate to the caller.
/// Autowiring will ignore any exceptions thrown by a filter method.
///
/// Unhandled exceptions thrown in a context will cause that context to be torn down.  By
/// the time the exception filter is called, teardown of the context originating the
/// exception is inevitable.  The originating context may potentially be torn down already
/// by the time the filter method is called.
///
/// All filter methods are guaranteed to be called in the thread and context of the code
/// which threw the exception.  In the case of Fire, it will be in the context of the
/// caller.  In the case of Defer, it will be in the context of the callee.
/// </remarks>
class ExceptionFilter
{
public:
  ExceptionFilter(void);
  virtual ~ExceptionFilter(void);

  /// <summary>
  /// This method is invoked when an exception has been thrown by CoreThread::Run
  /// </summary>
  /// <remarks>
  /// Such an exception is treated as a fatal exception, and will unconditionally result
  /// in the enclosing context being torn down.  Filters MUST NOT attempt to alter this
  /// behavior.  If this behavior is not desirable, greater care should be taken to
  /// prevent certain exceptions from being unhandled.
  ///
  /// Implementors can use "throw" with no arguments to trigger a rethrow of the originating exception.
  ///
  /// Exceptions thrown by this method are silently ignored by Autowiring.
  /// </remarks>
  virtual void Filter(void) {};

  /// <summary>
  /// This is an exception filter for exceptions thrown during a call to Fire
  /// </summary>
  /// <param name="pJunctionBox">The junction box generating the exception</param>
  /// <param name="pRecipient">The target of the call</param>
  /// <remarks>
  /// Implementors can use "throw" with no arguments to trigger a rethrow of the originating exception.
  ///
  /// Exceptions thrown by this method are silently ignored by Autowiring.
  /// </remarks>
  virtual void Filter(const JunctionBoxBase* pJunctionBox, CoreObject* pRecipient) {}
};
