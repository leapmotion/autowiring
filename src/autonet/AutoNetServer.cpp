// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoNetServer.h"

DefaultAutoNetTransport::DefaultAutoNetTransport(void){}
DefaultAutoNetTransport::~DefaultAutoNetTransport(void){}

void DefaultAutoNetTransport::Start(void) {
  
}

void DefaultAutoNetTransport::Stop(void) {
  
}

void DefaultAutoNetTransport::Send(connection_hdl hdl, const std::string& msg){
  
}

void DefaultAutoNetTransport::OnOpen(std::function<void(connection_hdl)> fn) {
  
}

void DefaultAutoNetTransport::OnClose(std::function<void(connection_hdl)> fn) {
  
}

void DefaultAutoNetTransport::OnMessage(std::function<void(connection_hdl, std::string)> fn) {
  
}

AutoNetServer::AutoNetServer():
  CoreThread("AutoNetServer")
{}

AutoNetServer::~AutoNetServer(){}

void AutoNetServer::AddEventHandlerInternal(const std::string& event, std::function<void(const std::vector<std::string>&)> handler) {
  std::lock_guard<std::mutex> lk(this->GetLock());
  m_handlers[event].push_back(std::move(handler));
}
