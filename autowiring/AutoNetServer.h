// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreThread.h"
#include "Autowired.h"
#include "AutowiringEvents.h"
#include "TypeRegistry.h"
#include <jzon/Jzon.h>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <map>
#include <set>

class Value;

class AutoNetServer:
  public CoreThread,
  public virtual AutowiringEvents
{
public:
  AutoNetServer();
  virtual ~AutoNetServer();

  //Types
  //NOTE: These types have undetermined template parameters,
  //so methods depending on these types must appear in the header.
  typedef websocketpp::server<websocketpp::config::asio> server;
  typedef server::message_ptr message_ptr;
  
  // Functions from CoreContext
  virtual void Run(void) override;
  virtual void OnStop(void) override;

  // Server Handler functions
  void OnOpen(websocketpp::connection_hdl hdl) {
    *this += [this, hdl] {
      SendMessage(hdl, "opened");
    };
  }
  void OnClose(websocketpp::connection_hdl hdl) {
    *this += [this, hdl] {
      SendMessage(hdl, "closed");
      this->m_Subscribers.erase(hdl);
    };
  }

  void OnMessage(websocketpp::connection_hdl hdl, message_ptr p_message);
  
  /// <summary>
  /// Waits until resume message is sent from the visualizer
  /// </summary>
  /// <param name="name">The identifier for this breakpoint</param>
  void Breakpoint(std::string name);

  /// <summary>
  /// Updates server when a new context is created
  /// </summary>
  /// <param name="ctxt">The new context</param>
  virtual void NewContext(CoreContext& ctxt) override;

  /// <summary>
  /// Updates server when a context has expired
  /// </summary>
  /// <param name="ctxt">The expired context</param>
  virtual void ExpiredContext(CoreContext& ctxt) override;

  /// <summary>
  /// Updates server when a new object is created
  /// </summary>
  /// <param name="ctxt">Context containing the object</param>
  /// <param name="obj">The object</param>
  virtual void NewObject(CoreContext& ctxt, const AnySharedPointer& obj) override;

  /// <summary>
  /// Updates server when a context has expired
  /// </summary>
  /// <param name="ctxt">The expired context</param>
  /// <param name="evtInfo">The event type</param>
  virtual void EventFired(CoreContext& ctxt, const std::type_info& evtInfo) override;

protected:
  /// <summary>
  /// Sends a message to specified client.
  /// </summary>
  /// <param name="hdl">Connection pointer on which to send message</param>
  /// <param name="pRecipient">Message name in CamelCase</param>
  /// <param name="args...">The first argument to be passed to client side event handler</param>
  /// <param name="args...">Remaining arguments to be passed to client side event handler</param>
  /// <remarks>
  /// Client callback with same number of arguments passed here will be called
  /// </remarks>
  template<typename Arg, typename... Args>
  void SendMessage(websocketpp::connection_hdl hdl, const char* p_type, Arg&& arg, Args&&... args){
    Jzon::Object msg;
    msg.Add("type", p_type);
    
    Jzon::Array arguments;
    bool dummy[] = {
      (arguments.Add(arg),false),
      (arguments.Add(args),false)...
    };
    (void)dummy;
    
    msg.Add("args", arguments);
    SendMessage(hdl, msg);
  }

  /// <summary>
  /// Sends a zero-argument message to specified client.
  /// </summary>
  void SendMessage(websocketpp::connection_hdl hdl, const char* p_type);

  /// <summary>
  /// Sends a zero-argument message to specified client.
  /// </summary>
  void SendMessage(websocketpp::connection_hdl hdl, const Jzon::Object& msg);

  /// <summary>
  /// Broadcast a message to all subscribers.
  /// </summary>
  /// <param name="pRecipient">Message name in CamelCase</param>
  /// <param name="args...">An arg to be passed to client side event handler</param>
  template<typename ...Args>
  void BroadcastMessage(const char* p_type, Args&&... args) {
    for (websocketpp::connection_hdl ptr : m_Subscribers)
      SendMessage(ptr, p_type, std::forward<Args>(args)...);
  }

  /// <summary>
  /// Called when a "Subscribe" event is sent from a client
  /// </summary>
  /// <param name="client">Client that sent event</param>
  void HandleSubscribe(websocketpp::connection_hdl hdl) {
    m_Subscribers.insert(hdl);

    Jzon::Array types;
    for (auto type : m_AllTypes) {
      types.Add(type.first);
    }

    SendMessage(hdl, "subscribed", types);
    AutoGlobalContext()->BuildCurrentState();

    // Send breakpoint message
    for (auto breakpoint : m_breakpoints) {
      SendMessage(hdl, "breakpoint", breakpoint);
    }
  }

  /// <summary>
  /// Called when a "Unsubscribe" event is sent from a client
  /// </summary>
  /// <param name="client">Client that sent event</param>
  void HandleUnsubscribe(websocketpp::connection_hdl hdl) {
    this->m_Subscribers.erase(hdl);
    SendMessage(hdl, "unsubscribed");
  }
  
  /// <summary>
  /// Called when a "terminateContext" event is sent from a client
  /// </summary>
  /// <param name="contextID">ID of the context to be terminated</param>
  void HandleTerminateContext(int contextID);
  
  /// <summary>
  /// Called when a "injectContextMember" event is sent from a client
  /// </summary>
  /// <param name="contextID">ID of the context to inject the type</param>
  /// <param name="typeName">Name of the type to inject. From std::type_info.name()</param>
  void HandleInjectContextMember(int contextID, std::string typeName);
  
  /// <summary>
  /// Called when a "injectContextMember" event is sent from a client
  /// </summary>
  /// <param name="name">Name of the breakpoint to resume</param>
  void HandleResumeFromBreakpoint(std::string name);

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
  
  
  ///////////// Member variables /////////////

  // Set of all subscribers
  std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> m_Subscribers;

  // one-to-one map of contexts to integers
  std::map<CoreContext*, int> m_ContextIDs;
  std::map<int, CoreContext*> m_ContextPtrs;

  // All event types
  std::set<std::shared_ptr<TypeIdentifierBase>> m_EventTypes;
  
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
  std::mutex m_mutex;
  std::condition_variable m_breakpoint_cv;
  std::set<std::string> m_breakpoints;
  
  // The actual server
  std::shared_ptr<server> m_Server;
  const int m_Port; // Port to listen on
};
