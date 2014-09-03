// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AutowiringConfig.h"
#include "AnySharedPointer.h"
#include <vector>

struct SatCounter;

/// <remarks>
/// A disposition holder, used to maintain initialization state on the key
/// </remarks>
struct DecorationDisposition
{
#if AUTOWIRING_USE_LIBCXX
  DecorationDisposition(DecorationDisposition&&) = delete;
  void operator=(DecorationDisposition&&) = delete;
  DecorationDisposition(const DecorationDisposition& source) = delete;
  void operator=(const DecorationDisposition& source) = delete;
#else
  // The methods below are needed for c++98 builds
  DecorationDisposition(DecorationDisposition&& source) :
    m_decoration(source.m_decoration),
    m_pImmediate(source.m_pImmediate),
    m_publisher(source.m_publisher),
    satisfied(source.satisfied),
    isCheckedOut(source.isCheckedOut),
    wasCheckedOut(source.wasCheckedOut)
  {}
  void operator=(DecorationDisposition&& source) {
    m_decoration = std::move(source.m_decoration);
    m_pImmediate = source.m_pImmediate;
    source.m_pImmediate = nullptr;
    m_publisher = source.m_publisher;
    satisfied = source.satisfied;
    isCheckedOut = source.isCheckedOut;
    wasCheckedOut = source.wasCheckedOut;
  }
  DecorationDisposition(const DecorationDisposition& source) :
    m_pImmediate(nullptr),
    m_publisher(source.m_publisher),
    satisfied(source.satisfied),
    isCheckedOut(source.isCheckedOut),
    wasCheckedOut(source.wasCheckedOut)
  {}
  void operator=(const DecorationDisposition& source) {
    m_decoration->reset();
    m_pImmediate = nullptr;
    m_publisher = source.m_publisher;
    satisfied = source.satisfied;
    isCheckedOut = source.isCheckedOut;
    wasCheckedOut = source.wasCheckedOut;
  }
#endif //AUTOWIRING_USE_LIBCXX

  DecorationDisposition(void) :
    m_pImmediate(nullptr),
    m_type(nullptr),
    m_publisher(nullptr),
    isCheckedOut(false),
    satisfied(false)
  {}

  // The decoration proper--potentially, this decoration might be from a prior execution of this
  // packet.  In the case of immediate decorations, this value will be invalid.
  AnySharedPointer m_decoration;

  // A pointer to the immediate decorations, if one is specified, or else nullptr
  const void* m_pImmediate;

  // The type of the decoration.
  const std::type_info* m_type;

  // Provider for this decoration, where it can be statically inferred.  Note that a provider for
  // this decoration may exist even if this value is null, in the event that dynamic decoration is
  // taking place.
  SatCounter* m_publisher;

  // Satisfaction counters, with the second part indicating a required entry if true,
  // or an optional entry if false.
  std::vector<std::pair<SatCounter*, bool>> m_subscribers;

  // Indicates that the internally held object is currently checked out,
  // but might not be satisfied, since the data is being prepared.
  bool isCheckedOut;

  // Flag indicating that this entry is satisfied
  // This implies that the entry has been previously checked out.
  // NOTE: In order to make a type unsatisfiable set (and persist)
  // isCheckedOut == true && satisfied == false
  bool satisfied;

  void Reset(void) {
    m_decoration->reset();
    m_pImmediate = nullptr;
    m_type = nullptr;
    isCheckedOut = false;
    satisfied = false;
  }
};
