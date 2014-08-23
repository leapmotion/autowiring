// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_SET
#include STL_UNORDERED_MAP

/// <summary>
/// Mutable properties used by AutoFilterDescriptor to describe data pipes.
/// </summary>
struct DataFlow {
  // DEFAULT: No data flow
  DataFlow() : broadcast(false) {}

  // Broadcast Input: AutoFilter accepts data from any input
  // Broadcast Output: Any AutoFilter can receive this data
  // Pipelined Input: AutoFilter only accepts data from declared pipes
  // Pipelined Output: AutoFilter only sends data to declared pipes
  bool broadcast;
  std::unordered_set<std::type_index> halfpipes;
};

/// Identifies the source fulfilling argument data.
/// Key is argument type, value is source type.
/// If the data is broadcast value will be &typeid(void)
typedef std::unordered_map<std::type_index, const std::type_info*> DataFill;
