#pragma once
#include "EventReceiver.h"

class AutoPacket;

class AutoPacketListener:
  public virtual EventReceiver
{
public:
  /// <summary>
  /// Invoked when a packet is being returned to this context's packet factory
  /// </summary>
  virtual void OnPacketReturned(const AutoPacket& packet) = 0;
};
