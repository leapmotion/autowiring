// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "JunctionBoxBase.h"
#include "JunctionBoxEntry.h"
#include "uuid.h"
#include <map>
#include <stdexcept>
#include <vector>
#include <cassert>
#include TYPE_INDEX_HEADER
#include MEMORY_HEADER
#include STL_UNORDERED_MAP
#include TYPE_TRAITS_HEADER

class CoreContext;
class JunctionBoxBase;

/// <summary>
/// General manager class of all junction boxes defined in some context
/// </summary>
class JunctionBoxManager {
public:
  JunctionBoxManager();
  virtual ~JunctionBoxManager();

  /// <summary>
  /// Get a JunctionBox by type
  /// </summary>
  std::shared_ptr<JunctionBoxBase> Get(const std::type_index& pTypeIndex);

  /// <summary>
  /// Allows this JunctionBox manager to begin processing events
  /// </summary>
  void Initiate(void);

  void AddEventReceiver(JunctionBoxEntry<Object> receiver);
  void RemoveEventReceiver(JunctionBoxEntry<Object> pRecvr);

protected:
  // All junction boxes known by this manager:
  typedef std::unordered_map<std::type_index, std::shared_ptr<JunctionBoxBase>> t_junctionBoxes;
  t_junctionBoxes m_junctionBoxes;
};
