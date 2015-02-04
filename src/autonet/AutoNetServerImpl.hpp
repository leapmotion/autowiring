// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoNetServer.h"
#include "AutowiringEvents.h"
#include <json11/json11.hpp>
#include <cctype>
#include <map>
#include <set>
#include SYSTEM_ERROR_HEADER
#include ARRAY_HEADER

#if !AUTOWIRING_USE_LIBCXX
  // No initializer lists on libstdc
  #define BOOST_NO_CXX11_HDR_INITIALIZER_LIST
#endif

// Need to redefine this namespace so we don't create linker problems.  Such problems may be
// created if our static library is imported by another project that uses an incompatible
// version of websocketpp.
#define websocketpp websocketpp_autonet
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/config/asio.hpp>

struct ObjectTraits;
struct TypeIdentifierBase;

template<typename ASIO>
class AutoNetServerImpl:
  public AutoNetServer,
  public virtual AutowiringEvents
{
public:
  AutoNetServerImpl();
  ~AutoNetServerImpl();

  //Types
  typedef websocketpp::server<ASIO> t_server;
  typedef typename t_server::message_ptr message_ptr;

  // Functions from BasicThread
  virtual void Run(void) override;
  virtual void OnStop(void) override;

  // Server Handler functions
  void OnOpen(websocketpp::connection_hdl hdl);
  void OnClose(websocketpp::connection_hdl hdl);
  void OnMessage(websocketpp::connection_hdl hdl, message_ptr p_message);
  
  // TLS Certificate
  void SetCertificate(const std::string& passphrase, const std::string& privateKeyPath, const std::string& certPath);

  // AutoNetServer overrides:
  void Breakpoint(std::string name) override;
  void HandleResumeFromBreakpoint(std::string name);

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
  virtual void NewObject(CoreContext& ctxt, const ObjectTraits& obj) override;

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
  void SendMessage(websocketpp::connection_hdl hdl, const char* p_type, Args&&... args){
    using json11::Json;

    Json msg = Json::object{
        {"type", p_type},
        {"args", Json::array{args...}}
    };

    m_Server.send(hdl, msg.dump(), websocketpp::frame::opcode::text);
  }

  /// <summary>
  /// Broadcast a message to all subscribers.
  /// </summary>
  /// <param name="pRecipient">Message name in CamelCase</param>
  /// <param name="args...">An arg to be passed to client side event handler</param>
  template<typename ...Args>
  void BroadcastMessage(const char* p_type, Args&&... args) {
    for(websocketpp::connection_hdl ptr : m_Subscribers)
      SendMessage(ptr, p_type, std::forward<Args>(args)...);
  }
  
  // Send custom event to all clients
  void SendEvent(const std::string& event, const std::vector<std::string>& args);

  /// <summary>
  /// Called when a "Subscribe" event is sent from a client
  /// </summary>
  /// <param name="client">Client that sent event</param>
  void HandleSubscribe(websocketpp::connection_hdl hdl);

  /// <summary>
  /// Called when a "Unsubscribe" event is sent from a client
  /// </summary>
  /// <param name="client">Client that sent event</param>
  void HandleUnsubscribe(websocketpp::connection_hdl hdl);

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
  std::mutex m_breakpoint_mutex;
  std::condition_variable m_breakpoint_cv;
  std::set<std::string> m_breakpoints;

  // The actual server
  t_server m_Server;
  const int m_Port; // Port to listen on
};

/// <summary>
/// Equivalent to new AutoNetServerImpl
/// </summary>
AutoNetServer* NewAutoNetServerImpl(AutoNetSecurity level);
