// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "SharedPointerSlot.h"

struct AnySharedPointer {
public:
  AnySharedPointer(void) {
    new (m_space) SharedPointerSlot;
  }

  explicit AnySharedPointer(const AnySharedPointer& rhs) {
    new (m_space) SharedPointerSlot(*rhs.slot());
  }

  template<class T>
  explicit AnySharedPointer(const std::shared_ptr<T>& rhs) {
    // Delegate the remainder to the assign operation:
    new (m_space) SharedPointerSlotT<T>(rhs);
  }

  ~AnySharedPointer(void) {
    // Pass control to the *real* destructor:
    slot()->~SharedPointerSlot();
  }

protected:
  unsigned char m_space[sizeof(SharedPointerSlot)];

public:
  // Convenience method to cast the space to a slot
  SharedPointerSlot* slot(void) { return (SharedPointerSlot*) m_space; }
  const SharedPointerSlot* slot(void) const { return (const SharedPointerSlot*) m_space; }

  explicit operator bool(void) const { return slot()->operator bool(); }

  SharedPointerSlot& operator*(void) { return *slot(); }
  const SharedPointerSlot& operator*(void) const { return *slot(); }

  SharedPointerSlot* operator->(void) { return slot(); }
  const SharedPointerSlot* operator->(void) const { return slot(); }

  template<class T>
  SharedPointerSlotT<T>& as(void) const { return (SharedPointerSlotT<T>&)*slot(); }

  bool operator==(const AnySharedPointer& rhs) const {
    return *slot() == *rhs.slot();
  }

  template<class T>
  bool operator==(const std::shared_ptr<T>& rhs) const {
    return *slot() == rhs;
  }

  /// <summary>
  /// Default for std library sorting of unique elements
  /// </summary>
  bool operator<(const AnySharedPointer& rhs) const { return *slot() < *rhs.slot();}

  /// <summary>
  /// Default for std library sorting of repeatable elements
  /// </summary>
  bool operator<=(const AnySharedPointer& rhs) const { return *slot() <= *rhs.slot();}

  bool operator>(const AnySharedPointer& rhs) const { return *slot() > *rhs.slot();}

  bool operator>=(const AnySharedPointer& rhs) const { return *slot() >= *rhs.slot();}

  /// <summary>
  /// Copy assignment operator
  /// </summary>
  /// <remarks>
  /// Consumer beware:  This is a transformative assignment.  The true polymorphic
  /// type will be carried from the right-hand side into this element, which is a
  /// different behavior from how things are normally done during assignment.  Other
  /// than that, however, the behavior is very similar to boost::any's assignment
  /// implementation.
  /// </remarks>
  SharedPointerSlot& operator=(const AnySharedPointer& rhs) {
    return **this = *rhs;
  }

  /// <summary>
  /// Convenience overload for shared pointer assignment
  /// </summary>
  template<class T>
  SharedPointerSlotT<T>& operator=(const std::shared_ptr<T>& rhs) {
    return **this = rhs;
  }
};

/// <summary>
/// Convenience implementation of AnySharedPointer which is initially of type T
/// </summary>
template<class T>
class AnySharedPointerT:
  public AnySharedPointer
{
public:
  AnySharedPointerT(void) {
    new (m_space) SharedPointerSlotT<T>();
  }

  // Convenience method to cast the space to a slot, but with the expected type
  SharedPointerSlotT<T>* slot(void) { return (SharedPointerSlotT<T>*) m_space; }
  const SharedPointerSlotT<T>* slot(void) const { return (const SharedPointerSlotT<T>*) m_space; }

  T& operator*(void) { return **slot(); }
  const T& operator*(void) const { return **slot(); }

  T* operator->(void) { return slot()->get().get(); }
  const T* operator->(void) const { return slot()->get().get(); }
};

template<class T>
inline bool operator==(const std::shared_ptr<T>& lhs, const AnySharedPointer& rhs) {
  return rhs == lhs;
}

static_assert(sizeof(AnySharedPointerT<int>) == sizeof(AnySharedPointer), "AnySharedPointer realization cannot have members");
static_assert(!std::is_polymorphic<AnySharedPointer>::value, "The shared pointer cannot be polymorphic, this prevents the root type from being aliased correctly");
