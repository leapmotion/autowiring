// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoNetServer.h"
#include "AutoNetServerImpl.hpp"

AutoNetServer::AutoNetServer():
  CoreThread("AutoNetServer")
{
}

AutoNetServer::~AutoNetServer(){}

AutoNetServer* AutoNetServer::New(void) {
  return new AutoNetServerImpl;
}
