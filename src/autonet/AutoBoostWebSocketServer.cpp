// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoBoostWebSocketServer.hpp"
#include "at_exit.h"
#include <iostream>
#include FUTURE_HEADER

using std::placeholders::_1;
using std::placeholders::_2;
using json11::Json;

AutoBoostWebSocketServer::AutoBoostWebSocketServer(Listener& listener) :
  m_Port(8000)
{
  // Configure websocketpp
  m_Server.init_asio();
  m_Server.set_access_channels(websocketpp::log::alevel::none);
  m_Server.set_error_channels(websocketpp::log::elevel::warn);

  // HACK: Work-around for AutoNet server shutdown
  // asio listen error: system:48 (Address already in use)
  m_Server.set_reuse_addr(true);

  // Register handlers
  m_Server.set_open_handler(std::bind(&Listener::OnOpen, &listener, ::_1));
  m_Server.set_close_handler(std::bind(&Listener::OnClose, &listener, ::_1));
  m_Server.set_message_handler(
    [this, &listener] (websocketpp::connection_hdl hdl, message_ptr p_message) {
      std::string err;
      if(!err.empty()) {
        std::cout << "Parse error: " << err << std::endl;
        SendClientMessage(hdl, "invalidMessage", "Couldn't parse message");
        return;
      }

      listener.OnMessage(hdl, Json::parse(p_message->get_payload(), err));
    }
  );
}

AutoBoostWebSocketServer::~AutoBoostWebSocketServer()
{
}

IWebSocketServer* IWebSocketServer::New(Listener& listener) {
  return new AutoBoostWebSocketServer(listener);
}

// CoreThread overrides
void AutoBoostWebSocketServer::Start(void){
  std::cout << "Starting Autonet server..." << std::endl;
  
  m_Server.listen(m_Port);
  m_Server.start_accept();
  
  // Blocks until the server finishes
  m_Server.run();
}

void AutoBoostWebSocketServer::Stop(void) {
  if (m_Server.is_listening())
    m_Server.stop_listening();
  
  m_Server.stop();
}


void AutoBoostWebSocketServer::Close(websocketpp::connection_hdl hdl) {
  m_Server.close(hdl, websocketpp::close::status::normal, "closed");
}