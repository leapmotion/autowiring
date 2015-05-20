// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutowiringConfig.h"
#include "AnySharedPointer.h"
#include <cassert>
#include <vector>
#include TYPE_INDEX_HEADER

struct SatCounter;

struct DecorationKey {
  DecorationKey(void) = default;

  DecorationKey(const DecorationKey& rhs) :
    ti(rhs.ti),
    is_shared(rhs.is_shared),
    tshift(rhs.tshift)
  {}
  
  explicit DecorationKey(const std::type_info& ti, bool is_shared, int tshift) :
    ti(&ti),
    is_shared(is_shared),
    tshift(tshift)
  {}
  
  // The type index
  const std::type_info* ti = nullptr;

  // True if this decoration can be used with AutoFilters that accept a shared_ptr input type
  bool is_shared = false;
  
  // Zero refers to a decoration created on this packet, a positive number [tshift] indicates
  // a decoration attached [tshift] packets ago.
  int tshift = -1;
  
  bool operator==(const DecorationKey& rhs) const {
    return ti == rhs.ti && is_shared == rhs.is_shared && tshift == rhs.tshift;
  }
};

namespace std {
  template<>
  struct hash<DecorationKey> {
    size_t operator()(const DecorationKey& key) const {
      return
        key.tshift +
        (key.is_shared ? 0x80000 : 0x70000) +
        key.ti->hash_code();
    }
  };
}

// The possible states for a DecorationDisposition
enum class DispositionState {
  // No decorations attached
  Unsatisfied,

  // Some decorations present, but not all of them.  Cannot proceed.
  PartlySatisfied,

  // Everything attached, ready to go
  Satisfied,

  // Unsatisfiable, and the callers on this decoration cannot accept a non-null
  // entry--IE, they accept const references as inputs.
  UnsatisfiableNoCall,

  // This decoration will never be satisfied.  Calls are generated to the
  Unsatisfiable
};

/// <remarks>
/// A disposition holder, used to maintain initialization state on the key
/// </remarks>
struct DecorationDisposition
{
  DecorationDisposition(void) = default;

  DecorationDisposition(const DecorationDisposition& source) :
    m_decorations(source.m_decorations),
    m_pImmediate(source.m_pImmediate),
    m_publishers(source.m_publishers),
    m_subscribers(source.m_subscribers),
    m_state(source.m_state)
  {}
  
  // The decoration proper--potentially, this decoration might be from a prior execution of this
  // packet.  In the case of immediate decorations, this value will be invalid.
  // Valid if and only if is_shared is false.
  std::vector<AnySharedPointer> m_decorations;

  // A pointer to the immediate decorations, if one is specified, or else nullptr.
  // Valid if and only if is_shared is true.
  const void* m_pImmediate = nullptr;

  // Providers for this decoration, where it can be statically inferred.  Note that a provider for
  // this decoration may exist even if this value is null, in the event that dynamic decoration is
  // taking place.
  std::vector<SatCounter*> m_publishers;

  // Satisfaction counters
  std::vector<SatCounter*> m_subscribers;

  // The current state of this disposition
  DispositionState m_state = DispositionState::Unsatisfied;
  
  /// <returns>
  /// True if all publishers have run on this disposition
  /// </summary>
  /// <remarks>
  /// Publication is complete automatically on this type if there are no statically known publishers,
  /// and at least one decoration has been attached to this disposition.
  /// </remarks>
  bool IsPublicationComplete(void) const {
    return
      !m_decorations.empty() &&
      m_decorations.size() >= m_publishers.size();
  }

  void Reset(void) {
    // IMPORTANT: Do not reset type_info
    m_decorations.clear();
    m_pImmediate = nullptr;
    m_state = DispositionState::Unsatisfied;
  }
};
