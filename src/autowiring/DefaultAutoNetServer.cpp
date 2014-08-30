#include "stdafx.h"
#include "AutoNetServer.h"

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