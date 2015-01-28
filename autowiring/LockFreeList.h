// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once

template<class T>
class LockFreeList {
public:
  // 2^6 = 64 maximum concurrent threads
  static const int sc_maxConcurrency = 6;

  // 2^26 = 67108864 (0x4000000) maximum entry count
  static const int sc_maxEntry = 32 - sc_maxConcurrency;

  // Utility masks:
  static const int sc_concurrencyMask = ((1 << sc_maxConcurrency) - 1) << sc_maxEntry;
  static const int sc_entryMax = (1 << sc_maxEntry) - 1;

  LockFreeList(void):
    m_pLookaside(nullptr)
  {
    m_sentry.transciever = 0;
    m_sentry.m_entry = nullptr;
  }

  ~LockFreeList(void) {
    // Destroy the whole list:
    Enumerate([] (T* val) {
      delete val;
    });

    // Delete the lookaside list proper:
    if(m_pLookaside)
      delete m_pLookaside;
  }

  union Link {
    // The total number of entities attempting to dereference the link:
    int transciever;

    // Link value--either constant, or internally synchronized
    T* m_entry;
  };

private:
  // Sentry link:
  Link m_sentry;

  // Link lookaside list:
  Link* m_pLookaside;

public:
  /// <summary>
  /// Enumeration routine which invokes the passed function on each internal member
  /// </summary>
  template<class Fn>
  void Enumerate(const Fn& fn) {
    // Start with the link sentry:
    Link* cur = &m_sentry;

    // Traffic increment:
    while(cur) {
      // TODO:  Interlocked increment
      break;
    }
  }
};
