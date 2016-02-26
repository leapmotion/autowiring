// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutoPacket.h"

/// <summary>
/// Internal representation type for AutoPacket, provides methods for exclusive use with a packet factory
/// <summary>
class AutoPacketInternal:
  public AutoPacket
{
public:
  // Must hold the lock to 'factory' when calling this constructor
  AutoPacketInternal(AutoPacketFactory& factory, std::shared_ptr<void>&& outstanding);
  ~AutoPacketInternal(void);

  /// <summary>
  /// Decrements subscribers requiring AutoPacket argument then calls all initializing subscribers.
  /// </summary>
  /// <remarks>
  /// Initialize is called when a packet is issued by the AutoPacketFactory.
  /// It is not called when the Packet is created since that could result in
  /// spurious calls when no packet is issued.
  /// </remarks>
  void Initialize(bool isFirstPacket, uint64_t uniqueId);

  /// <summary>
  /// 
  /// </summary>
  std::shared_ptr<AutoPacketInternal> SuccessorInternal(void);
};

