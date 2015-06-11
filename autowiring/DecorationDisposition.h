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
    tshift(rhs.tshift)
  {}
  
  explicit DecorationKey(const std::type_info& ti, int tshift) :
    ti(&ti),
    tshift(tshift)
  {}
  
  // The type index
  const std::type_info* ti = nullptr;

  // Zero refers to a decoration created on this packet, a positive number [tshift] indicates
  // a decoration attached [tshift] packets ago.
  int tshift = -1;
  
  bool operator==(const DecorationKey& rhs) const {
    return ti == rhs.ti && tshift == rhs.tshift;
  }
};

namespace std {
  template<>
  struct hash<DecorationKey> {
    size_t operator()(const DecorationKey& key) const {
      return key.tshift + key.ti->hash_code();
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

  // This decoration will never be satisfied.  Calls are generated with a null
  // shared pointer passed as the value.
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
  struct Subscriber {
    Subscriber(void) {}

    Subscriber(bool is_optional, SatCounter* satCounter):
      is_optional{is_optional},
      satCounter{satCounter}
    {}

    // Optional flag.  If this flag is set, it indicates that the referenced AutoFilter could still
    // be called even if the decoration is not attached to the packet--IE, the AutoFilter accepts a
    // shared pointer or an array
    bool is_optional = false;

    // The satisfaction counter itself
    SatCounter* satCounter = nullptr;
  };

  std::vector<Subscriber> m_subscribers;

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
