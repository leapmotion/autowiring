// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include <json11/json11.hpp>
#include <websocketpp/server.hpp>

#ifdef _MSC_VER
  #ifdef AUTOWIRING_EXPORT_AUTONET
    #define AUTONET_EXPORT __declspec(dllexport)
  #else
    #define AUTONET_EXPORT __declspec(dllimport)
  #endif
#else
  #define AUTONET_EXPORT
#endif

class IWebSocketServer {
public:
  ~IWebSocketServer(void) {}

  class Listener {
  public:
    // Server Handler functions
    virtual void OnOpen(websocketpp::connection_hdl hdl) = 0;
    virtual void OnClose(websocketpp::connection_hdl hdl) = 0;
    virtual void OnMessage(websocketpp::connection_hdl hdl, const json11::Json& msg) = 0;
  };

  static AUTONET_EXPORT IWebSocketServer* New(Listener& listener);

  // Connection-oriented callbacks:

  // Start and stop routines for the underlying webserver
  virtual void Start(void) = 0;
  virtual void Stop(void) = 0;
  virtual void Close(websocketpp::connection_hdl hdl) = 0;

  /// <summary>
  /// Sends the specified JSON message to the client
  /// </summary>
  virtual void SendClientMessageJson(websocketpp::connection_hdl hdl, const char* p_type, const json11::Json& msg) = 0;

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
  void SendClientMessage(websocketpp::connection_hdl hdl, const char* p_type, Args&&... args) {
    using json11::Json;

    Json msg = Json::object{
        {"type", p_type},
        {"args", Json::array{args...}}
    };
    SendClientMessageJson(hdl, p_type, msg);
  }
};