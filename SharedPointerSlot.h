#pragma once
#include "fast_pointer_cast.h"
#include "Object.h"
#include MEMORY_HEADER
#include <stddef.h>

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
  SharedPointerSlot(void) {}

  SharedPointerSlot(const SharedPointerSlot& rhs) {
    *this = rhs;
  }

  template<class T>
  SharedPointerSlot(const std::shared_ptr<T>& rhs) {
    // Delegate the remainder to the assign operation:
    *this = rhs;
  }

  // Base destructor, doesn't do anything because there's nothing to release from
  // the base class
  virtual ~SharedPointerSlot(void) {}

protected:
  // Space, used to store a shared pointer--by default, though, it's just empty.
  unsigned char m_space[sizeof(std::shared_ptr<void>)];

  /// <summary>
  /// Assignment routine
  /// </summary>
  /// <remarks>
  /// If this method is called, there will be a strong guarantee that the type of rhs
  /// will be precisely equal to the type of this.
  ///
  /// Implementors MUST treat this as a type of constructor replacement.  The contents
  /// of m_space are indeterminate and certainly invalid, and should be treated as
  /// uninitialized memory.
  /// </remarks>
  virtual void assign(const SharedPointerSlot& rhs) {}

public:
  virtual operator void*(void) const { return nullptr; }
  virtual operator std::shared_ptr<Object>(void) const { return std::shared_ptr<Object>(); }

  /// <returns>
  /// True if this slot holds nothing
  /// </returns>
  virtual bool empty(void) const { return true; }

  /// <returns>
  /// True if this pointer slot holds an instance of the specified type
  /// </returns>
  template<class T>
  bool is(void) const { return type() == typeid(T); }

  /// <returns>
  /// Returns the type of the shared pointer held in this slot, or typeid(void) if empty
  /// </returns>
  virtual const std::type_info& type(void) const { return typeid(void); }

  /// <summary>
  /// Clears this type, if a shared pointer is currently held
  /// </summary>
  void reset(void) {
    if(empty())
      // Nothing to do, just back out
      return;
  }

  /// <summary>
  /// Attempts to coerce this type to the speceified type
  /// </summary>
  template<class T>
  std::shared_ptr<T> as(void) const {
    if(type() == typeid(void))
      // This is allowed, we always permit null to be cast to the requested type.
      return std::shared_ptr<T>();

    if(type() != typeid(T))
      throw std::runtime_error("Attempted to obtain a shared pointer for an unrelated type");

    return ((SharedPointerSlotT<T>*)this)->get();
  }

  /// <summary>
  /// In-place polymorphic transformer
  /// </summary>
  template<class T>
  void operator=(const std::shared_ptr<T>& rhs) {
    if(type() == typeid(T)) {
      // We can just use the equivalence operator, no need to make two calls
      *((SharedPointerSlotT<T>*)this) = rhs;
      return;
    }

    if(!empty())
      // We aren't presently empty, we need to release our
      // current implementation before attempting to reinitialize
      this->~SharedPointerSlot();

    // Now we can safely reinitialize:
    new (this) SharedPointerSlotT<T>(rhs);
  }

  /// <summary>
  /// Specialization for the Object base type
  /// </summary>
  bool operator==(const std::shared_ptr<Object>& rhs) const {
    return this->operator std::shared_ptr<Object>() == rhs;
  }

  template<class T>
  bool operator==(const std::shared_ptr<T>& rhs) const {
    if(empty())
      // If we're empty, then we are equal if the rhs is also empty
      // This means we're basically comparing null to null, and we're going to
      // generally allow it.
      return rhs == nullptr;

    // If we're not empty, then we will verify that types match before attempting any
    // other comparison.  This means that it's possible for types related via a dynamic
    // cast to fail a comparison--callers must be aware of this possibility.
    if(type() != typeid(*rhs))
      return false;

    // Everything lines up, coerce ourselves to the derived type and handoff the
    // comparison behavior.
    return (SharedPointerSlotT<T>&)*this == rhs;
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
  void operator=(const SharedPointerSlot& rhs) {
    // Our own stuff is going away, need to return here
    reset();

    // If the right-hand side is empty, we'll just reset ourselves and be done with it:
    if(rhs.empty())
      return;

    // We want to scrape the VFT, which always appears prior to the data block.  We need
    // to use pointer magic to find the address of the VFT.  On most systems it's just
    // one void* space at the top of the class, but we can't be too careful here.
    size_t headerSpace = (char*) m_space - (char*) this;
    memcpy(this, &rhs, headerSpace);

    // If we had a way to know the compile-time type of the rhs, we might have simply
    // done a placement new on ourselves and that would have been enough to initialize
    // our internally held shared pointer.  Unfortunately, we don't know the type on
    // the right-hand side, and can't do a placement new on the type.  Instead, we will
    // use our protected virtual method "assign" to do this operation.  The implementation
    // of assign expects that the passed value will have matching types, and since we just
    // made our types symmetric with the above memcpy, we can be assured we will get the
    // correct version of assign.
    assign(rhs);
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
    new (m_space) std::shared_ptr<T>(rhs);
  }

  ~SharedPointerSlotT(void) {
    // Recast and in-place destroy our shared pointer:
    get().~shared_ptr();
  }

protected:
  void assign(const SharedPointerSlot& rhs) override {
    // Static cast rhs to our own type, we know a priori that the passed
    // value will match so we elide the typical safety checks.
    auto& rhsCasted = static_cast<const SharedPointerSlotT&>(rhs);

    // And now it's just a matter of copying things over.
    new (m_space) std::shared_ptr<T>(rhsCasted.get());
  }

public:
  /// <returns>
  /// The shared pointer held by this slot
  /// </returns>
  std::shared_ptr<T>& get(void) { return *(std::shared_ptr<T>*)m_space; }
  const std::shared_ptr<T>& get(void) const { return *(std::shared_ptr<T>*)m_space; }

  virtual operator std::shared_ptr<Object>(void) const override {
    return leap::fast_pointer_cast<Object>(get());
  }

  virtual bool empty(void) const { return get() == nullptr; }
  virtual operator const void*(void) const { return get().get(); }
  const std::type_info& type(void) const override { return typeid(T); }

  template<class U>
  bool operator==(const std::shared_ptr<U>& rhs) const {
    return get() == rhs;
  }

  // We have a better opeartor overload for type T:
  void operator=(const std::shared_ptr<T>& rhs) {
    get() = rhs;
  }
};

struct AnySharedPointer {
public:
  AnySharedPointer(void) {
    new (m_space) SharedPointerSlot;
  }

  AnySharedPointer(const AnySharedPointer& rhs) {
    new (m_space) SharedPointerSlot(rhs.slot());
  }

  template<class T>
  AnySharedPointer(const std::shared_ptr<T>& rhs) {
    // Delegate the remainder to the assign operation:
    new (m_space) SharedPointerSlot(rhs);
  }

  ~AnySharedPointer(void) {
    // Pass control to the *real* destructor:
    slot().~SharedPointerSlot();
  }

private:
  unsigned char m_space[sizeof(SharedPointerSlot)];

  // Convenience method to cast the space to a slot
  SharedPointerSlot& slot(void) { return *(SharedPointerSlot*) m_space; }
  const SharedPointerSlot& slot(void) const { return *(SharedPointerSlot*) m_space; }

public:
  operator void*(void) const { return slot().operator void*(); }

  SharedPointerSlot& operator*(void) { return slot(); }
  const SharedPointerSlot& operator*(void) const { return slot(); }

  SharedPointerSlot* operator->(void) { return &slot(); }
  const SharedPointerSlot* operator->(void) const { return &slot(); }

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
    slot().~SharedPointerSlot();
    *(SharedPointerSlot*) m_space = rhs.slot();
  }
};

static_assert(!std::is_polymorphic<AnySharedPointer>::value, "The shared pointer cannot be polymorphic");
