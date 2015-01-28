// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutowiringConfig.h"
#include "AnySharedPointer.h"
#include <cassert>
#include <vector>
#include TYPE_INDEX_HEADER

struct SatCounter;

struct DecorationKey {
  DecorationKey(const DecorationKey& rhs) :
    ti(rhs.ti),
    tshift(rhs.tshift)
  {}
  
  explicit DecorationKey(const std::type_info& ti, int tshift = 0) :
    ti(ti),
    tshift(tshift)
  {}
  
  // The type index
  const std::type_info& ti;
  
  // Zero refers to a decoration created on this packet, a positive number [tshift] indicates
  // a decoration attached [tshift] packets ago.
  int tshift;
  
  bool operator==(const DecorationKey& rhs) const {
    return ti == rhs.ti && tshift == rhs.tshift;
  }
};

namespace std {
  template<>
  struct hash<DecorationKey> {
    size_t operator()(const DecorationKey& key) const {
#if AUTOWIRING_USE_LIBCXX
      return key.ti.hash_code() + key.tshift;
#else
      return std::type_index(key.ti).hash_code() + key.tshift;
#endif
    }
  };
}

// The possible states for a DecorationDisposition
enum class DispositionState {
  Unsatisfied,
  PartlySatisfied,
  Satisfied,
  Unsatisfiable
};

/// <remarks>
/// A disposition holder, used to maintain initialization state on the key
/// </remarks>
struct DecorationDisposition
{
#if AUTOWIRING_USE_LIBCXX
  DecorationDisposition(DecorationDisposition&&) = delete;
  void operator=(DecorationDisposition&&) = delete;
#else
  // The methods below are needed for c++98 builds
  DecorationDisposition(DecorationDisposition&& source) :
    m_decorations(source.m_decorations),
    m_pImmediate(source.m_pImmediate),
    m_publishers(source.m_publishers),
    m_state(source.m_state)
  {}
  DecorationDisposition& operator=(DecorationDisposition&& source) {
    m_decorations = std::move(source.m_decorations);
    m_pImmediate = source.m_pImmediate;
    source.m_pImmediate = nullptr;
    m_publishers = std::move(source.m_publishers);
    m_state = source.m_state;
    return *this;
  }
#endif //AUTOWIRING_USE_LIBCXX

  DecorationDisposition(void) :
    m_type(nullptr),
    m_pImmediate(nullptr),
    m_state(DispositionState::Unsatisfied)
  {}

  DecorationDisposition(const DecorationDisposition& source) :
    m_type(source.m_type),
    m_pImmediate(source.m_pImmediate),
    m_publishers(source.m_publishers),
    m_subscribers(source.m_subscribers),
    m_state(source.m_state)
  {}

  DecorationDisposition& operator=(const DecorationDisposition& source) {
    m_type = source.m_type;
    m_pImmediate = source.m_pImmediate;
    m_publishers = source.m_publishers;
    m_subscribers = source.m_subscribers;
    m_state = source.m_state;
    return *this;
  }

private:
  // Destructured key for this decoration. Use accessor functions to access
  // This is needed because DecorationKey doesn't have a default constructor
  const std::type_info* m_type;
  int m_tshift;
  
public:
  // The decoration proper--potentially, this decoration might be from a prior execution of this
  // packet.  In the case of immediate decorations, this value will be invalid.
  std::vector<AnySharedPointer> m_decorations;

  // A pointer to the immediate decorations, if one is specified, or else nullptr
  const void* m_pImmediate;

  // Providers for this decoration, where it can be statically inferred.  Note that a provider for
  // this decoration may exist even if this value is null, in the event that dynamic decoration is
  // taking place.
  std::vector<SatCounter*> m_publishers;

  // Satisfaction counters
  std::vector<SatCounter*> m_subscribers;

  // The current state of this disposition
  DispositionState m_state;
  
  // Set the key that identifies this decoration
  void SetKey(const DecorationKey& key) {
    m_type = &key.ti;
    m_tshift = key.tshift;
  }
  
  // Get the key that identifies this decoration
  DecorationKey GetKey(void) const {
    assert(m_type);
    return DecorationKey(*m_type, m_tshift);
  }
  
  operator bool() {
    return !!m_type;
  }

  void Reset(void) {
    // IMPORTANT: Do not reset type_info
    m_decorations.clear();
    m_pImmediate = nullptr;
    m_state = DispositionState::Unsatisfied;
  }
};
