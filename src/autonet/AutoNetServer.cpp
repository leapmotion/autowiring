// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoNetServer.h"

AutoNetServer::AutoNetServer():
  CoreThread("AutoNetServer")
{}

AutoNetServer::~AutoNetServer(){}

void AutoNetServer::AddEventHandler(const std::string& event, std::function<void(const std::vector<std::string>&)> handler) {
  *this += [this, event, handler] {
    m_handlers[event].push_back(std::move(handler));
  };
}
