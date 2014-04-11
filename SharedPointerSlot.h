#pragma once
#include SHARED_PTR_HEADER

template<class T>
struct SharedPointerSlotT;

/// <summary>
/// Specialized space-invariant implementation of boost::any for shared pointers
/// </summary>
/// <remarks>
/// This implementation is superior to boost::any because it does not require the
/// use of the heap for allocations, and is slightly faster under teardown for
/// the same reason.
/// </remarks>
struct SharedPointerSlot {
public:
  // Base destructor, doesn't do anything because there's nothing to release from
  // the base class
  virtual ~SharedPointerSlot(void) {}

protected:
  // Space, used to store a shared pointer--by default, though, it's just empty.
  unsigned char space[sizeof(std::shared_ptr<void>)];

public:
  virtual operator void*(void) const {
    return nullptr;
  }

  /// <summary>
  /// In-place polymorphic transformer
  /// </summary>
  template<class T>
  SharedPointerSlotT<T>& operator=(const std::shared_ptr<T>& rhs) {
    if(typeid(*this) == typeid(SharedPointerSlotT<T>))
      // We can just use the equivalence operator, no need to make two calls
      return *((SharedPointerSlotT<T>*)this) = rhs;

    if(typeid(*this) != typeid(SharedPointerSlot))
      // We are currently a different type.  We need to release our
      // current implementation before attempting to recast.
      this->~SharedPointerSlot();

    // Recast and reassign:
    return *new (this) SharedPointerSlotT<T>(rhs);
  }
};

template<class T>
struct SharedPointerSlotT:
  SharedPointerSlot
{
  SharedPointerSlotT(const std::shared_ptr<T>& rhs) {
    static_assert(
      sizeof(SharedPointerSlotT<T>) == sizeof(SharedPointerSlot),
      "Slot instance is too large to fit in the base type"
    );

    // Make use of our space to make a shared pointer:
    new (space) std::shared_ptr<T>(rhs);
  }

  ~SharedPointerSlotT(void) {
    // Recast and in-place destroy our shared pointer:
    ((std::shared_ptr<T>*)space)->~shared_ptr();
  }

  virtual operator void*(void) const {
    return ((std::shared_ptr<T>*)space)->get();
  }

  // We have a better opeartor overload for type T:
  SharedPointerSlotT& operator=(const std::shared_ptr<T>& rhs) {
    *((std::shared_ptr<T>*)space) = rhs;
    return *this;
  }
};