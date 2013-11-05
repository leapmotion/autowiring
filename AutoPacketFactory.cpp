#include "stdafx.h"
#include "AutoPacketFactory.h"
#include "AutoPacket.h"
#include "AutoPacketListener.h"

void AutoPacketFactory::AutoPacketResetter::operator()(AutoPacket& packet) const {
  // Notify all listeners that a packet has just returned home:
  m_apl(&AutoPacketListener::OnPacketReturned)(packet);

  // Eliminate references to any shared pointers
  packet.Release();
}

AutoPacketFactory::AutoPacketFactory()
{
}

AutoPacketFactory::~AutoPacketFactory()
{
}

std::shared_ptr<AutoPacket> AutoPacketFactory::NewPacket(void) {
  // Obtain a packet:
  std::shared_ptr<AutoPacket> retVal;
  m_packets(retVal);

  // Fill the packet with satisfaction information:
  retVal->Reset();

  // Done, return
  return retVal;
}