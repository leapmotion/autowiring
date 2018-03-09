// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#pragma once
#include "altitude.h"
#include "AnySharedPointer.h"
#include <atomic>
#include <set>
#include <vector>

namespace autowiring {

struct SatCounter;

struct DecorationKey {
  DecorationKey(void) = default;

  explicit DecorationKey(auto_id id) :
    id(id)
  {}

  // The type index
  auto_id id;

  bool operator==(const DecorationKey& rhs) const {
    return id == rhs.id;
  }
};

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

  // Providers for this decoration, where it can be statically inferred.  Note that a provider for
  // this decoration may exist even if this value is null, in the event that dynamic decoration is
  // taking place.
  std::vector<SatCounter*> m_publishers;

  /// <summary>
  /// Represents a filter on this decoration that actually changes the decoration.
  /// </summary>
  struct Modifier {
    Modifier(void) = default;

    Modifier(bool is_shared, autowiring::altitude altitude, SatCounter* satCounter) :
      is_shared{is_shared},
      altitude{altitude},
      satCounter{satCounter}
    {
      if (satCounter == nullptr)
        throw std::runtime_error("Cannot initialize Modifier with nullptr to SatCounter.");
    }

    Modifier(const Modifier& rhs) = default;

    Modifier& operator=(const Modifier& rhs) = default;

    // True if a shared pointer will be taken, false otherwise
    bool is_shared;

    // The altitude of the satisfaction counter
    autowiring::altitude altitude;

    // The pointer to the satisfaction counter, it should never be nullptr
    SatCounter* satCounter;
  };

  // Modifiers of this decoration, ordered by altitude.  These modifiers do not all become ready to
  // run at the same time, and they MUST NOT be run concurrently with each other.
  std::vector<Modifier> m_modifiers;

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

    Subscriber(void) = default;

    Subscriber(bool is_shared, Type type, autowiring::altitude altitude, SatCounter* satCounter) :
      is_shared{is_shared},
      type{type},
      altitude{altitude},
      satCounter{satCounter}
    {
      if (satCounter == nullptr)
        throw std::runtime_error("Cannot initialize Subscriber with nullptr to SatCounter.");
    }

    // True if a shared pointer will be taken, false otherwise
    const bool is_shared;

    // The relationship between this subscriber and the provided decoration
    const Type type;

    // The altitude of the satisfaction counter
    const autowiring::altitude altitude;

    // The pointer to the satisfaction counter, it should never be nullptr
    SatCounter* const satCounter;

    bool operator<(const Subscriber& rhs) const {
      return std::tie(altitude, satCounter) < std::tie(rhs.altitude, rhs.satCounter);
    }
  };

  // Subscribers of this decoration, sorted by altitude.
  std::set<Subscriber> m_subscribers;

  // The current state of this disposition
  DispositionState m_state = DispositionState::Unsatisfied;

  /// <summary>
  /// Increments the number of producers run by one
  /// </summary>
  /// <returns>True if the decoration has become satisfied as a result of this call</returns>
  bool IncProducerCount(void) {
    auto producersRun = ++m_nProducersRun;

    // Uniformly advance state:
    switch (m_state) {
    case DispositionState::Unsatisfied:
    case DispositionState::PartlySatisfied:
      // Permit a transition to another state
      if (producersRun >= m_publishers.size()) {
        m_state = DispositionState::Complete;
        return true;
      }

      m_state = DispositionState::PartlySatisfied;
      break;
    default:
      // Do nothing, no advancing to any states from here
      break;
    }
    return false;
  }

  /// <returns>
  /// True if all publishers have run on this disposition
  /// </summary>
  /// <remarks>
  /// Publication is complete whenever all producers have run on the decoration.
  /// </remarks>
  bool IsPublicationComplete(void) const {
    return m_nProducersRun >= m_publishers.size();
  }

  void Reset(void) {
    // IMPORTANT: Do not reset type_info
    m_decorations.clear();
    m_pImmediate = nullptr;
    m_state = DispositionState::Unsatisfied;
  }
};

}

namespace std {
  template<>
  struct hash<autowiring::DecorationKey> {
    size_t operator()(const autowiring::DecorationKey& key) const {
      return (size_t)key.id.block;
    }
  };
}
