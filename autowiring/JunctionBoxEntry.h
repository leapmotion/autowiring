// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include <stdexcept>
#include MEMORY_HEADER

class CoreContext;

struct JunctionBoxEntryBase {
  JunctionBoxEntryBase(CoreContext* owner) :
    m_owner(owner)
  {}

  CoreContext* const m_owner;
};

/// <summary>
/// Utility structure representing a junction box entry together with its owner
/// </summary>
template<class T>
struct JunctionBoxEntry:
  JunctionBoxEntryBase
{
  JunctionBoxEntry(CoreContext* owner, std::shared_ptr<T> ptr) :
    JunctionBoxEntryBase(owner),
    m_ptr(ptr)
  {}

  std::shared_ptr<T> m_ptr;

  JunctionBoxEntry& operator=(const JunctionBoxEntry& rhs) {
    // This shouldn't be used. non-c++11 containers require this...
    throw std::runtime_error("Can't copy a JunctionBoxEntry");
  }

  bool operator==(const JunctionBoxEntry& rhs) const {
    return m_ptr == rhs.m_ptr;
  }

  bool operator<(const JunctionBoxEntry& rhs) const {
    return m_ptr < rhs.m_ptr;
  }

  operator bool(void) const {
    return !!m_ptr.get();
  }

  template<class U>
  JunctionBoxEntry<U> Rebind(void) const {
    return JunctionBoxEntry<U>(
      m_owner,
      std::dynamic_pointer_cast<U, T>(m_ptr)
    );
  }
};

namespace std {
  /// <summary>
  /// Hash specialization for the junction box entry
  /// </summary>
  template<class T>
  struct hash<JunctionBoxEntry<T>> :
    public hash<std::shared_ptr<T>>
  {
    size_t operator()(const JunctionBoxEntry<T>& jbe) const {
      return hash<std::shared_ptr<T>>::operator()(jbe.m_ptr);
    }
  };
}
