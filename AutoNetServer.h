// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once
#include "Autowiring/AutowiringEvents.h"
#include "Autowiring/TypeRegistry.h"
#include "Autowiring/CoreThread.h"
#include "Jzon.h"

#include <websocketpp/websocketpp.hpp>
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
  typedef websocketpp::server::connection_ptr connection_ptr;
  typedef websocketpp::server::handler::message_ptr message_ptr;
  
  // Functions from CoreContext
  virtual void Run(void) override;
  virtual void OnStop(void) override;

  // Server Handler functions
  void OnOpen(connection_ptr);
  void OnClose(connection_ptr);
  void OnMessage(connection_ptr, message_ptr);
  
  /// <summary>
  /// Waits untill resume message is sent from the visualizer
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
  /// <param name="p_connection">Connection pointer on which to send message</param>
  /// <param name="pRecipient">Message name in CamelCase</param>
  /// <param name="args...">The first argument to be passed to client side event handler</param>
  /// <param name="args...">Remaining arguments to be passed to client side event handler</param>
  /// <remarks>
  /// Client callback with same number of arguments passed here will be called
  /// </remarks>
  template<typename Arg, typename... Args>
  void SendMessage(connection_ptr p_connection, const char* p_type, Arg&& arg, Args&&... args){
    Jzon::Object msg;
    msg.Add("type", p_type);
    
    Jzon::Array arguments;
    bool dummy[] = {
      (arguments.Add(arg),false),
      (arguments.Add(args),false)...
    };
    (void)dummy;
    
    msg.Add("args", arguments);
    
    Jzon::Writer writer(msg, Jzon::NoFormat);
    writer.Write();
    p_connection->send(writer.GetResult());
  }

  /// <summary>
  /// Sends a zero-argument message to specified client.
  /// </summary>
  void SendMessage(connection_ptr p_connection, const char* p_type){
    Jzon::Object msg;
    msg.Add("type", p_type);
    Jzon::Array arguments;
    msg.Add("args", arguments);
    
    Jzon::Writer writer(msg, Jzon::NoFormat);
    writer.Write();
    p_connection->send(writer.GetResult());
  }

  /// <summary>
  /// Broadcast a message to all subscribers.
  /// </summary>
  /// <param name="pRecipient">Message name in CamelCase</param>
  /// <param name="args...">An arg to be passed to client side event handler</param>
  template<typename ...Args>
  void BroadcastMessage(const char* p_type, Args&&... args) {
    for (connection_ptr ptr : m_Subscribers)
      SendMessage(ptr, p_type, std::forward<Args>(args)...);
  }

  /// <summary>
  /// Called when a "Subscribe" event is sent from a client
  /// </summary>
  /// <param name="client">Client that sent event</param>
  void HandleSubscribe(connection_ptr client);

  /// <summary>
  /// Called when a "Unsubscribe" event is sent from a client
  /// </summary>
  /// <param name="client">Client that sent event</param>
  void HandleUnsubscribe(connection_ptr client);
  
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
  void PollCoreThreadUtilization(boost::chrono::milliseconds period);
  
  
  ///////////// Member variables /////////////

  // Set of all subscribers
  std::set<connection_ptr> m_Subscribers;

  // one-to-one map of contexts to integers
  std::map<CoreContext*, int> m_ContextIDs;
  std::map<int, CoreContext*> m_ContextPtrs;

  // All event types
  std::set<std::shared_ptr<TypeIdentifierBase>> m_EventTypes;
  
  // All ContextMembers
  std::map<std::string, std::function<void(void)>> m_AllTypes;
  
  // All CoreThreads
  std::set<std::weak_ptr<CoreThread>, std::owner_less<std::weak_ptr<CoreThread>>> m_CoreThreads;
  
  // Breakpoint functionality
  boost::mutex m_mutex;
  boost::condition_variable m_breakpoint_cv;
  std::set<std::string> m_breakpoints;
  
  // The actual server
  std::shared_ptr<websocketpp::server> m_Server;
  const int m_Port; // Port to listen on
  
  // Handler wrapper. Injects "OnOpen", "OnClose", and "OnMessage" into the websocketpp server
  class Handler :
    public websocketpp::server::handler
  {
  public:
    Handler(AutoNetServer& SocketServer) : m_SocketServer(SocketServer) {}
    
    virtual void on_open(connection_ptr con){ m_SocketServer.OnOpen(con); };
    virtual void on_close(connection_ptr con){ m_SocketServer.OnClose(con); };
    virtual void on_message(connection_ptr con, message_ptr msg){ m_SocketServer.OnMessage(con,msg); };
    
  private:
    AutoNetServer& m_SocketServer;
  };
};
