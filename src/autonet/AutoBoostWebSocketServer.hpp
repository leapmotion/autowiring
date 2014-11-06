// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "IWebSocketServer.hpp"
#include <cctype>
#include <future>
#include <map>
#include <set>

// Need to redefine this namespace so we don't create linker problems.  Such problems may be
// created if our static library is imported by another project that uses an incompatible
// version of websocketpp.
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

struct ObjectTraits;
struct TypeIdentifierBase;

class AutoBoostWebSocketServer:
  public IWebSocketServer
{
public:
  AutoBoostWebSocketServer(Listener& listener);
  ~AutoBoostWebSocketServer();

  //Types
  typedef websocketpp::server<websocketpp::config::asio> t_server;
  typedef t_server::message_ptr message_ptr;

  // Functions from BasicThread
  void Start(void) override;
  void Stop(void) override;
  void Close(websocketpp::connection_hdl hdl) override;

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
  void SendClientMessageJson(websocketpp::connection_hdl hdl, const char* p_type, const json11::Json& msg) override  {
    m_Server.send(hdl, msg.dump(), websocketpp::frame::opcode::text);
  }

  /// <summary>
  /// Append a lambda to this queue that will poll CoreThreads for their utilization
  /// </summary>
  void PollThreadUtilization(std::chrono::milliseconds period);


  /*******************************************
  *             Member variables             *
  *******************************************/

  // The actual server
  t_server m_Server;
  const int m_Port; // Port to listen on
};
