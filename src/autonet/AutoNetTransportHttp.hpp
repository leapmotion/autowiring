// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoNetServer.h"
#include MEMORY_HEADER
#include MUTEX_HEADER
#include <array>
#include <set>

// Need to redefine this namespace so we don't create linker problems.  Such problems may be
// created if our static library is imported by another project that uses an incompatible
// version of websocketpp.
#define websocketpp websocketpp_autonet
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

/// <summary>
/// Default transport layer for AutoNet
/// </summary>
/// <remarks>
/// This transport implementation provides a very simple unsecure HTTP server implementation
/// </remarks>
class AutoNetTransportHttp:
  public AutoNetTransport
{
public:
  typedef std::weak_ptr<void> connection_hdl;

  AutoNetTransportHttp();
  virtual ~AutoNetTransportHttp();

  void Start(void) override;
  void Stop(void) override;

  void Send(connection_hdl hdl, const std::string& msg) override;
  void SetTransportHandler(std::shared_ptr<AutoNetTransportHandler> handler) override;

private:
  typedef websocketpp::server<websocketpp::config::asio> t_server;
  typedef t_server::message_ptr message_ptr;

  // Transport handler reference
  std::shared_ptr<AutoNetTransportHandler> m_handler;

  std::mutex m_lock;
  std::set<connection_hdl, std::owner_less<connection_hdl>> m_connections;
  int m_port;
  t_server m_server;
};
