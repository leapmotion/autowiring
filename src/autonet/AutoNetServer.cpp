// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoNetServer.h"

AutoNetServer::AutoNetServer():
  CoreThread("AutoNetServer")
{}

AutoNetServer::~AutoNetServer(){}

void AutoNetServer::AddEventHandlerInternal(const std::string& event, std::function<void(const std::vector<std::string>&)> handler) {
  std::lock_guard<std::mutex> lk(this->GetLock());
  m_handlers[event].push_back(std::move(handler));
}
