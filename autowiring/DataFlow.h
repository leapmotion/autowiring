// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include TYPE_INDEX_HEADER
#include STL_UNORDERED_SET
#include STL_UNORDERED_MAP

namespace autowiring {
  /// <summary>
  /// Mutable properties used by AutoFilterDescriptor to describe data pipes.
  /// </summary>
  struct DataFlow {
    // DEFAULT: No data flow
    DataFlow() :
      output(false),
      broadcast(false)
    {}

    // Broadcast Output: Any AutoFilter can receive this data
    // Pipelined Output: AutoFilter only sends data to declared pipes
    // Broadcast Input: AutoFilter accepts data from any input
    // Pipelined Input: AutoFilter only accepts data from declared pipes
    bool output;
    bool broadcast;
    std::unordered_set<std::type_index> halfpipes;
  };

/// Identifies the source fulfilling argument data.
/// Key is argument type, value is source type.
/// If the data is broadcast value will be &typeid(void)
  class DataFill:
    public std::unordered_map<std::type_index, const std::type_info*>
  {
  public:
    template<class Arg>
    const std::type_info* source() const {
      std::unordered_map<std::type_index, const std::type_info*>::const_iterator itr = find(typeid(Arg));
      if (itr != end())
        return itr->second;
      return &typeid(void);
    }
  };
}