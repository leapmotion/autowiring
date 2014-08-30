// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutoNetServer.h"
#include <string>

/// <summary>
/// A default AutoNet implementation which does nothing
/// </summary>
class DefaultAutoNetServer:
  public AutoNetServer
{
public:
  DefaultAutoNetServer(void);

  void Breakpoint(std::string name) {}
  void HandleResumeFromBreakpoint(std::string name) {}
};

#if !AUTOWIRING_BUILD_AUTONET
AutoNetServer* NewAutoNetServerImpl(void) {
  return new DefaultAutoNetServer;
}
#endif
