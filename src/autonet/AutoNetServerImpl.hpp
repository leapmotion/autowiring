// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoNetServer.h"
#include <json11/json11.hpp>
#include <cctype>
#include <map>
#include <set>
#include SYSTEM_ERROR_HEADER
#include ARRAY_HEADER

namespace autowiring {
  struct CoreObjectDescriptor;
}

// Protocol layer for AutoNet
class AutoNetServerImpl:
  public AutoNetServer,
  public AutoNetTransportHandler
{
public:
  /// <summary>
  /// Constructs an AutoNet server with the default HTTP transport mechanism
  /// </summary>
  AutoNetServerImpl(void);

  /// <summary>
  /// Constructs an AutoNet server with the specified transport
  /// </summary>
  AutoNetServerImpl(std::unique_ptr<AutoNetTransport>&& transport);
  ~AutoNetServerImpl();

  // Handle websocket messages
  void OnOpen(connection_hdl handle) override {
  }
  void OnClose(connection_hdl handle) override {
    *this += [this, handle] {
      this->m_Subscribers.erase(handle);
    };
  }
  void OnMessage(AutoNetTransportHandler::connection_hdl hdl, const std::string& payload) override;

  // Functions from BasicThread
  virtual void Run(void) override;
  virtual void OnStop(void) override;

  // AutoNetServer overrides:
  void Breakpoint(std::string name) override;
  void HandleResumeFromBreakpoint(std::string name);

  /// <summary>
  /// Updates server when a new context is created
  /// </summary>
  /// <param name="pParent">The parent context, or nullptr if one does not exist</param>
  /// <param name="newCtxt">The new context</param>
  void NewContext(CoreContext* pParent, CoreContext& newCtxt);

  /// <summary>
  /// Updates server when a context has expired
  /// </summary>
  /// <param name="ctxt">The expired context</param>
  void ExpiredContext(CoreContext& ctxt);

  /// <summary>
  /// Updates server when a new object is created
  /// </summary>
  /// <param name="ctxt">Context containing the object</param>
  /// <param name="obj">The object</param>
  void NewObject(CoreContext& ctxt, const autowiring::CoreObjectDescriptor& obj);

protected:
  /// <summary>
  /// Sends a message to specified client.
  /// </summary>
  /// <param name="hdl">Connection pointer on which to send message</param>
  /// <param name="pRecipient">Message name in CamelCase</param>
  /// <param name="args...">Arguments to be passed to client side event handler</param>
  /// <remarks>
  /// Client callback with same number of arguments passed here will be called
  /// </remarks>
  template<typename... Args>
  void SendMessage(AutoNetTransportHandler::connection_hdl hdl, const char* p_type, Args&&... args) {
    using json11::Json;

    Json msg = Json::object{
        {"type", p_type},
        {"args", Json::array{args...}}
    };

    m_transport->Send(hdl, msg.dump());
  }

  /// <summary>
  /// Broadcast a message to all subscribers.
  /// </summary>
  /// <param name="pRecipient">Message name in CamelCase</param>
  /// <param name="args...">An arg to be passed to client side event handler</param>
  template<typename ...Args>
  void BroadcastMessage(const char* p_type, Args&&... args) {
    for(auto ptr : m_Subscribers)
      SendMessage(ptr, p_type, std::forward<Args>(args)...);
  }

  // Send custom event to all clients
  void SendEvent(const std::string& event, const std::vector<std::string>& args) override;

  /// <summary>
  /// Called when a "Subscribe" event is sent from a client
  /// </summary>
  /// <param name="client">Client that sent event</param>
  void HandleSubscribe(AutoNetTransportHandler::connection_hdl hdl);

  /// <summary>
  /// Called when a "Unsubscribe" event is sent from a client
  /// </summary>
  /// <param name="client">Client that sent event</param>
  void HandleUnsubscribe(AutoNetTransportHandler::connection_hdl hdl);

  /// <summary>
  /// Assigns each context a unique ID number
  /// </summary>
  /// <param name="ctxt">Client that sent event</param>
  int ResolveContextID(CoreContext* ctxt);
  CoreContext* ResolveContextID(int id);

  /// <summary>
  /// Append a lambda to this queue that will poll CoreThreads for their utilization
  /// </summary>
  void PollThreadUtilization(std::chrono::milliseconds period);


  /*******************************************
  *             Member variables             *
  *******************************************/

  // Set of all subscribers
  std::set<AutoNetTransportHandler::connection_hdl, std::owner_less<AutoNetTransportHandler::connection_hdl>> m_Subscribers;

  // one-to-one map of contexts to integers, and a lock because we use this in an unsynchronized setting
  autowiring::spin_lock m_lock;
  std::map<CoreContext*, int> m_ContextIDs;
  std::map<int, CoreContext*> m_ContextPtrs;

  // All ContextMembers
  std::map<std::string, std::function<void(void)>> m_AllTypes;

  // All CoreThreads
  struct ThreadStats {
    // Last amount of time the thread was known to be running
    std::chrono::milliseconds m_lastRuntimeKM;
    std::chrono::milliseconds m_lastRuntimeUM;
  };
  std::map<std::weak_ptr<BasicThread>, ThreadStats, std::owner_less<std::weak_ptr<BasicThread>>> m_Threads;

  // Breakpoint functionality
  std::mutex m_breakpoint_mutex;
  std::condition_variable m_breakpoint_cv;
  std::set<std::string> m_breakpoints;

  // Transport layer for AutoNet
  std::unique_ptr<AutoNetTransport> m_transport;
};

/// <summary>
/// Equivalent to new AutoNetServerImpl
/// </summary>
AutoNetServer* NewAutoNetServerImpl(std::unique_ptr<AutoNetTransport>);
AutoNetServer* NewAutoNetServerImpl(void);
