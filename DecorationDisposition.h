#pragma once
#include "AnySharedPointer.h"
#include <vector>

struct SatCounter;

/// <remarks>
/// A disposition holder, used to maintain initialization state on the key
/// </remarks>
struct DecorationDisposition
{
  DecorationDisposition(void) :
    m_pImmediate(nullptr),
    satisfied(false),
    isCheckedOut(false),
    wasCheckedOut(false)
  {}

  // The decoration proper--potentially, this decoration might be from a prior execution of this
  // packet.  In the case of immediate decorations, this value will be invalid.
  AnySharedPointer m_decoration;

  // A pointer to the immediate decorations, if one is specified, or else nullptr
  const void* m_pImmediate;

  // Satisfaction counters, with the second part indicating a required entry if true,
  // or an optional entry if false.
  std::vector<std::pair<SatCounter*, bool>> m_subscribers;

  // Flag indicating that this entry is satisfied
  bool satisfied;

  // Indicates that the internally held object is currently checked out
  bool isCheckedOut;

  // Indicates that this entry was checked out at some point in the past, and can no longer be
  // checked out for a subsequent satisfaction
  bool wasCheckedOut;

  void Reset(void) {
    m_decoration->reset();
    m_pImmediate = nullptr;
    satisfied = false;
    isCheckedOut = false;
    wasCheckedOut = false;
  }
};
