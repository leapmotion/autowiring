#pragma once
#include "SharedPointerSlot.h"

struct AnySharedPointer {
public:
  AnySharedPointer(void) {
    new (m_space) SharedPointerSlot;
  }

  AnySharedPointer(const AnySharedPointer& rhs) {
    new (m_space) SharedPointerSlot(*rhs.slot());
  }

  template<class T>
  AnySharedPointer(const std::shared_ptr<T>& rhs) {
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

  bool operator==(const AnySharedPointer& rhs) const {
    return *slot() == *rhs.slot();
  }

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
  void operator=(const AnySharedPointer& rhs) {
    **this = *rhs;
  }

  /// <summary>
  /// Convenience overload for shared pointer assignment
  /// </summary>
  template<class T>
  void operator=(const std::shared_ptr<T>& rhs) {
    **this = rhs;
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
};

template<class T>
inline bool operator==(const std::shared_ptr<T>& lhs, const AnySharedPointer& rhs) {
  return rhs == lhs;
}

static_assert(!std::is_polymorphic<AnySharedPointer>::value, "The shared pointer cannot be polymorphic");
