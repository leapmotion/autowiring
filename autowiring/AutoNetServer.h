// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "CoreThread.h"

class AutoNetServer;

extern AutoNetServer* NewAutoNetServerImpl(void);

class AutoNetServer:
  public CoreThread
{
protected:
  AutoNetServer();

public:
  virtual ~AutoNetServer();

  static AutoNetServer* New(void) {
    return NewAutoNetServerImpl();
  }

  /// <summary>
  /// Waits until resume message is sent from the visualizer
  /// </summary>
  /// <param name="name">The identifier for this breakpoint</param>
  virtual void Breakpoint(std::string name) = 0;

  // Allows a breakpoint previously set with Breakpoint to be resumed
  virtual void HandleResumeFromBreakpoint(std::string name) = 0;
};
