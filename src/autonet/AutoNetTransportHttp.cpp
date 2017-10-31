// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoNetTransportHttp.hpp"

AutoNetTransportHttp::AutoNetTransportHttp(void) :
  m_port(8000)
{
  m_server.init_asio();
  m_server.set_access_channels(websocketpp::log::alevel::none);
  m_server.set_error_channels(websocketpp::log::elevel::none);


  m_server.set_open_handler([this](connection_hdl hdl) {
    auto handler =
      (
        (std::lock_guard<std::mutex>)m_lock,
        m_connections.insert(hdl),
        m_handler
      );
    if (m_handler)
      m_handler->OnOpen(hdl);
  });

  m_server.set_close_handler([this](connection_hdl hdl) {
    auto handler =
      (
        (std::lock_guard<std::mutex>)m_lock,
        m_connections.erase(hdl),
        m_handler
      );

    m_handler->OnClose(hdl);
  });

  m_server.set_message_handler([this](websocketpp::connection_hdl hdl, message_ptr message){
    auto handler = ((std::lock_guard<std::mutex>)m_lock, m_handler);

    std::string msg = message->get_payload();
    handler->OnMessage(hdl, msg);
  });
}

AutoNetTransportHttp::~AutoNetTransportHttp(void)
{
}

void AutoNetTransportHttp::Start(void) {
  m_server.listen(m_port);
  m_server.start_accept();
  m_server.run();
}

void AutoNetTransportHttp::Stop(void) {
  if (m_server.is_listening())
    m_server.stop_listening();

  for (auto& conn : m_connections)
    m_server.close(conn, websocketpp::close::status::normal, "closed");
}

void AutoNetTransportHttp::Send(connection_hdl hdl, const std::string& msg){
  m_server.send(hdl, msg, websocketpp::frame::opcode::text);
}

void AutoNetTransportHttp::SetTransportHandler(std::shared_ptr<AutoNetTransportHandler> handler) {
  std::lock_guard<std::mutex> lk(m_lock);
  m_handler = handler;
}
