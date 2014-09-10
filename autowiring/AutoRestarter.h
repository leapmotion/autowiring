// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "atomic_object.h"
#include "CoreRunnable.h"
#include "ExceptionFilter.h"

struct AutoRestarterConfig
{
  AutoRestarterConfig(void) :
    restartOnException(false),
    restartOnShutdown(false),
    startWhenCreated(false)
  {}

  // Restart the context on exception
  bool restartOnException;

  // Restart the context if it terminates through ordinary behavior.  This will continue
  // until the AutoRestarter's own exterior context has been terminated.
  //
  // If the context is manually shut down, it then the AutoRestarter will return nullptr
  // in response to GetContext until Regenerate is called
  bool restartOnShutdown;

  // Once the context is created (or recreated), start it up
  bool startWhenCreated;
};

/// <summary>
/// Holds a shared pointer to a CoreContext and monitors that context for termination
/// </summary>
/// <remarks>
/// The behavior of this type is configurable on construction.  This type always creates
/// the Sigil-holding context in its constructor, but will wait until the outer scope is
/// initialized before attempting to start the child context up.
///
/// This type does not prevent other contexts with a matching Sigil from being created
/// in the current context.
/// </remarks>
template<class Sigil>
class AutoRestarter:
  public CoreRunnable,
  public ExceptionFilter
{
public:
  AutoRestarter(AutoRestarterConfig& config):
    config(config)
  {
    GenerateContext();
  }

  const AutoRestarterConfig config;

  // CoreRunnable overrides:
  bool Start(std::shared_ptr<Object> outstanding) override {
    // Start the enclosed context, do nothing else
    auto ctxt = GetContext();
    if(ctxt && config.startWhenCreated)
      ctxt->Initiate();
    return true;
  }

  void Stop(bool graceful) override {
    std::lock_guard<std::mutex> lk(m_lock);
    m_context.reset();
  }
  bool IsRunning(void) const override { return false; }
  bool ShouldStop(void) const override { return true; }
  void Wait(void) override {}

private:
  mutable std::mutex m_lock;
  std::shared_ptr<CoreContext> m_context;

  class Monitor:
    public ContextMember,
    public CoreRunnable
  {
  public:
    Monitor(AutoRestarter<Sigil>& ar) :
      ar(ar)
    {}

    // Parent restarter, we hand control here when we're stopped
    AutoRestarter<Sigil>& ar;

    bool Start(std::shared_ptr<Object> outstanding) override {
      m_outstanding = outstanding;
      return true;
    }

    void Stop(bool graceful) override {
      ar.OnContextStopped(*this);
      m_outstanding.reset();
    }

    std::shared_ptr<Object> m_outstanding;

    bool IsRunning(void) const override { return false; }
    bool ShouldStop(void) const override { return false; }
    void Wait(void) {}
  };

protected:
  void GenerateContext(void) {
    // Create the new context before releasing the old one
    AutoCreateContextT<Sigil> context;
    AutoRequired<Monitor> arm(context, *this);

    // Swap over our shared pointer
    bool bIsRestarting;
    {
      std::lock_guard<std::mutex> lk(m_lock);
      bIsRestarting = m_context != nullptr;
      m_context = context;
    }

    // Start up only if Generate is being called in a regenerative setting, otherwise
    // we don't want to try to initiate before our own enclosing context is started.
    if(bIsRestarting && config.startWhenCreated)
      context->Initiate();
  }

  /// <summary>
  /// Called by the monitor context member when a context has been stopped
  /// </summary>
  virtual void OnContextStopped(const ContextMember& monitor) {
    {
      std::lock_guard<std::mutex> lk(m_lock);
      if(m_context != monitor.GetContext())
        // Only reset if we need to
        return;
      m_context.reset();
    }

    if(config.restartOnShutdown)
      GenerateContext();
  }

  void Filter(void) override {
    if(config.restartOnException)
      GenerateContext();
  }

  void Filter(const JunctionBoxBase* pJunctionBox, Object* pRecipient) override {
    Filter();
  }

public:
  /// <returns>
  /// A valid context pointer to a context with sigil type Sigil
  /// </returns>
  /// <remarks>
  /// Bolt users beware:  This method will always return an out-of-date value, or possibly even
  /// nullptr, because bolts are fired before the context pointer is returned to the creating
  /// entity.
  ///
  /// This will return nullptr if the current context has been terminated
  /// </remarks>
  std::shared_ptr<CoreContext> GetContext(void) const {
    return std::lock_guard<std::mutex>(m_lock), m_context;
  }

  /// <summary>
  /// Shuts down the attached interior context and creates a new one
  /// </summary>
  void Regenerate(void) {
    auto ctxt = GetContext();
    if(ctxt)
      ctxt->SignalShutdown();

    if(!config.restartOnShutdown)
      // Won't restart automatically, we have to do things ourselves
      GenerateContext();
  }
};