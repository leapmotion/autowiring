#include "stdafx.h"
#include "AutoPacketFactory.h"
#include "AutoPacket.h"

AutoPacketFactory::AutoPacketFactory():
  m_numSats(0)
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

  // Done, return
  return retVal;
}