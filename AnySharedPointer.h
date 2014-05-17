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
  explicit AnySharedPointer(const std::shared_ptr<T>& rhs) {
    // Delegate the remainder to the assign operation:
    new (m_space) SharedPointerSlotT<T>(rhs);
  }

  ~AnySharedPointer(void) {
    // Pass control to the *real* destructor:
    slot()->~SharedPointerSlot();
  }

private:
  unsigned char m_space[sizeof(SharedPointerSlot)];

  // Convenience method to cast the space to a slot
  SharedPointerSlot* slot(void) { return (SharedPointerSlot*) m_space; }
  const SharedPointerSlot* slot(void) const { return (const SharedPointerSlot*) m_space; }

public:
  operator bool(void) const { return slot()->operator bool(); }

  SharedPointerSlot& operator*(void) { return *slot(); }
  const SharedPointerSlot& operator*(void) const { return *slot(); }

  SharedPointerSlot* operator->(void) { return slot(); }
  const SharedPointerSlot* operator->(void) const { return slot(); }

  template<class T>
  SharedPointerSlotT<T>& as(void) const { return (SharedPointerSlotT<T>&)*slot(); }

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

static_assert(!std::is_polymorphic<AnySharedPointer>::value, "The shared pointer cannot be polymorphic");
