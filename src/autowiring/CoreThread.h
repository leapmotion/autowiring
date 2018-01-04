// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include "BasicThread.h"
#include "DispatchQueue.h"
#include MEMORY_HEADER

class CoreContext;
class CoreThread;

/// <summary>
/// Provides a dispatch queue that creates a thread to run jobs (in the form of
/// lambda functions) in the order added.
/// </summary>
/// <remarks>
/// Create a CoreThread object by autowiring it to a context. You can then use the
/// overloaded "+=" operator to add lambda functions to the dispatch queue. The
/// core thread instance executes the functions in order.
///
/// \include snippets/CoreThread_Class.txt
///
/// When the CoreThread object receives the signal to shutdown, i.e from
/// CoreContext::SignalShutdown(bool graceful), shutdown behavior depends on the
/// graceful parameter. For graceful shutdown, the class completes the current
/// job queue, but does not allow new jobs to be added to the queue. For immediate
/// shutdown, the job queue is abandoned and the current job aborted.
/// </remarks>
class CoreThread:
  public BasicThread,
  public DispatchQueue
{
public:
  /// Constructs a core thread object with an optional name.
  ///
  /// The name assigned to a thread is visible in some debuggers.
  /// <param name="pName">An optional name for this thread.</param>
  CoreThread(const char* pName = nullptr);
  virtual ~CoreThread(void);

protected:
  /// <summary>
  /// While stopping, make sure we do it cleanly
  /// </summary>
  std::mutex m_stoppingLock;
  std::condition_variable m_stoppingCond;
  bool m_onStopCompleted = false;

  /// <summary>
  /// Overridden here so we can rundown the dispatch queue
  /// </summary>
  virtual void DoRunLoopCleanup(std::shared_ptr<CoreContext>&& ctxt, std::shared_ptr<CoreObject>&& refTracker) override;

public:
  /// \internal
  /// <summary>
  /// Called automatically to begin core thread execution.
  /// </summary>
  /// <param name="context">A shared pointer to the context containing this thread</param>
  /// <remarks>
  /// The default implementation of Run will simply call WaitForEvent in a loop until it's
  /// told to quit.
  /// </remarks>
  void Run(void) override;

  /// <summary>
  /// Invoked when the thread should stop. Implement this method to perform any needed cleanup.
  /// </summary>
  /// <remarks>
  /// This method is called before the dispatch queue is aborted or run down.  Users wishing to perform
  /// operations gracefully during termination should pend these operations as lambdas to the thread's
  /// dispatch queue; these lambdas will be invoked if graceful termination is requested, and destroyed
  /// without invocation otherwise.
  ///
  /// Do not perform any time-consuming operations in this callback; the method may be called
  /// from a time-sensitive context and unacceptable system performance could result if long-duration
  /// operations are undertaken here.
  ///
  /// The base implementation of this method is guaranteed to do nothing.
  /// </remarks>
  void OnStop(void) override {}

  /// <summary>
  /// Invoked when the thread should stop. Override to perform custom handling when the thread is told to stop.
  /// </summary>
  /// <param name="graceful">Set to true to rundown the dispatch queue before quitting.</param>
  /// <remarks>
  /// This method is called when the thread should stop.  When invoked, the value of
  /// CoreThread::ShouldStop is guaranteed to be true.
  ///
  /// The default behavior is to terminate the job queue immediately if graceful is false.
  /// Otherwise, existing jobs in the queue are executed, but no new functions
  /// can be added to the dispatch queue.
  ///
  /// Derived classes are not required to call CoreThread::OnStop.
  /// </remarks>
  void OnStop(bool graceful) override final;
};
