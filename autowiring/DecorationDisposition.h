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
    id(rhs.id),
    tshift(rhs.tshift)
  {}
  
  explicit DecorationKey(auto_id id, int tshift) :
    id(id),
    tshift(tshift)
  {}
  
  // The type index
  auto_id id;

  // Zero refers to a decoration created on this packet, a positive number [tshift] indicates
  // a decoration attached [tshift] packets ago.
  int tshift = -1;
  
  bool operator==(const DecorationKey& rhs) const {
    return id == rhs.id && tshift == rhs.tshift;
  }
};

namespace std {
  template<>
  struct hash<DecorationKey> {
    size_t operator()(const DecorationKey& key) const {
      return key.tshift + (size_t)key.id.block;
    }
  };
}

// The possible states for a DecorationDisposition
enum class DispositionState {
  // No decorations attached
  Unsatisfied,

  // Some decorations present, but not all of them.  Cannot proceed.
  PartlySatisfied,

  // All publishers on this decoration have been run or cannot be executed
  Complete
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
    m_pModifier(source.m_pModifier),
    m_publishers(source.m_publishers),
    m_subscribers(source.m_subscribers),
    m_state(source.m_state)
  {}
  
  // The number of producers of this decoration type which have concluded.  This number may be larger
  // than the number of attached decorations if some producers could not run.
  size_t m_nProducersRun = 0;

  // The decorations proper--potentially, these decorations might be from a prior execution of this
  // packet.  In the case of immediate decorations, this value will be invalid.
  // Valid if and only if is_shared is false.
  std::vector<AnySharedPointer> m_decorations;

  // A pointer to the immediate decorations, if one is specified, or else nullptr.
  // Valid if and only if is_shared is true.
  const void* m_pImmediate = nullptr;

  // Modifier for this decoration, if one is available, or else nullptr.
  SatCounter* m_pModifier = nullptr;

  // Providers for this decoration, where it can be statically inferred.  Note that a provider for
  // this decoration may exist even if this value is null, in the event that dynamic decoration is
  // taking place.
  std::vector<SatCounter*> m_publishers;

  // Satisfaction counters
  struct Subscriber {
    enum class Type {
      // Ordinary, reference-in, non-optional subscriber
      Normal,

      // Optional.  If this flag is set, it indicates that the referenced AutoFilter could still be
      // called even if the decoration is not attached to the packet--IE, the AutoFilter accepts a
      // shared pointer or an array
      Optional,

      // Abides by all of the rules of Optional, but additionally can be called when multiple
      // instances of a decoration are present.
      Multi
    };

    Subscriber(void) {}
    Subscriber(bool is_shared, Type type, SatCounter* satCounter) :
      is_shared{is_shared},
      type{type},
      satCounter{satCounter}
    {}

    // True if a shared pointer will be taken, false otherwise
    bool is_shared = false;

    // The relationship between this subscriber and the provided decoration
    Type type = Type::Normal;

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
    return !m_decorations.empty() && m_nProducersRun >= m_publishers.size();
  }

  void Reset(void) {
    // IMPORTANT: Do not reset type_info
    m_decorations.clear();
    m_pImmediate = nullptr;
    m_state = DispositionState::Unsatisfied;
  }
};
