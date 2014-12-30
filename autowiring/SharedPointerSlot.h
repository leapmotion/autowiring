// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_id.h"
#include "autowiring_error.h"
#include "fast_pointer_cast.h"
#include "Object.h"
#include "SlotInformation.h"
#include MEMORY_HEADER
#include <stdexcept>

template<class T, bool use_object = true>
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

  explicit SharedPointerSlot(const SharedPointerSlot& rhs) {
    *this = rhs;
  }

  template<class T>
  explicit SharedPointerSlot(const std::shared_ptr<T>& rhs) {
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
  operator bool(void) const { return !empty(); }
  virtual operator std::shared_ptr<Object>(void) const { return std::shared_ptr<Object>(); }
  virtual void* ptr(void) { return nullptr; }
  virtual const void* ptr(void) const { return nullptr; }

  /// <summary>
  /// Performs a placement new on the specified space with a type matching the current instance
  /// </summary>
  /// <remarks>
  /// This method will also initialize the returned space with a copy of the shared pointer held by this
  /// slot.
  /// </remarks>
  virtual void New(void* pSpace, size_t nBytes) const {
    if(nBytes < sizeof(*this))
      throw std::runtime_error("Attempted to construct a SharedPointerSlot in a space that was too small");
    new (pSpace) SharedPointerSlot;
  }

  /// <returns>
  /// A void pointer to the underlying shared pointer implementation
  /// </returns>
  /// <remarks>
  /// Use this method with great caution.  The return value may be safely cast to type std::shared_ptr<T>
  /// _if_ the correct type for T is known at compile time, but if this pointer is shared at runtime between
  /// modules, compilation differences can change the layout of the shared pointer in subtle ways.
  /// Generally speaking, this function should only be called by modules which are guaranteed to have
  /// been statically linked in the same executable.
  /// </remarks>
  const void* shared_ptr(void) const { return m_space; }

  /// <summary>
  /// Attempts to dynamically assign this slot to the specified object without changing the current type
  /// </summary>
  /// <returns>True if the assignment succeeds</returns>
  virtual bool try_assign(const std::shared_ptr<Object>& rhs) {
    return false;
  }

  /// <summary>
  /// Alters the type of this slot to match the specified type
  /// </summary>
  /// <param name="T">The type to initialize this slot into</param>
  /// <remarks>
  /// This operation releases any previously held value, and causes the slot to hold nullptr with
  /// the specified type
  /// </remarks>
  template<class T>
  SharedPointerSlotT<T>& init(void) {
    // Release what we're holding
    reset();

    // Reinitialize, now that the slot is empty
    return *new (this) SharedPointerSlotT<T>();
  }

  /// <returns>
  /// True if this slot holds nothing
  /// </returns>
  virtual bool empty(void) const { return true; }

  /// <returns>
  /// True if this pointer slot holds an instance of the specified type
  /// </returns>
  template<class T>
  bool is(void) const { return type() == typeid(auto_id<T>); }

  /// <returns>
  /// Returns the template type of the shared pointer held in this slot, or typeid(void) if empty
  /// </returns>
  virtual const std::type_info& type(void) const { return typeid(void); }

  /// <summary>
  /// Clears this type, if a shared pointer is currently held
  /// </summary>
  /// <remarks>
  /// This method will preserve the polymorphic type of this slot--IE, it does not change the return
  /// value of this->type()
  /// </remarks>
  virtual void reset(void) {}

  /// <summary>
  /// Attempts to coerce this type to the specified type
  /// </summary>
  template<class T>
  const std::shared_ptr<T>& as(void) const {
    static const std::shared_ptr<T> s_empty;

    if (type() == typeid(void))
      // This is allowed, we always permit null to be cast to the requested type.
      return s_empty;

    if (type() != typeid(auto_id<T>))
      throw std::runtime_error("Attempted to obtain a shared pointer for an unrelated type");

    // Instantiate the static cast with "false" because this function should not be attempting to
    // instantiate any casts.
    return static_cast<const SharedPointerSlotT<T, false>*>(this)->get();
  }

  /// <summary>
  /// Identical to as(), but performs no type safety checks
  /// </summary>
  template<class T>
  const std::shared_ptr<T>& as_unsafe(void) const {
    static const std::shared_ptr<T> s_empty;

    if (type() == typeid(void))
      // This is allowed, we always permit null to be cast to the requested type.
      return s_empty;

    // Instantiate the static cast with "false" because this function should not be attempting to
    // instantiate any casts.
    return static_cast<const SharedPointerSlotT<T, false>*>(this)->get();
  }

  /// <summary>
  /// Comparison by reference. Comparison of unequal types always fails,
  /// even when different type casts of the same instance are referenced.
  /// </summary>
  bool operator==(const SharedPointerSlot& rhs) const {
    // Unequal types are always unequal
    if(type() != rhs.type())
      return false;
    return ptr() == rhs.ptr();
  }

  /// <summary>
  /// Comparison by reference. Comparison of unequal types always fails,
  /// even when different type casts of the same instance are referenced.
  /// </summary>
  template<class T>
  bool operator==(const std::shared_ptr<T>& rhs) const {
    // Unequal types are always unequal
    if(type() != typeid(auto_id<T>))
      return false;

    // Everything lines up, coerce ourselves to the derived type and handoff the
    // comparison behavior.
    return (SharedPointerSlotT<T>&)*this == rhs;
  }

  /// <summary>
  /// Specialization for the Object base type
  /// </summary>
  bool operator==(const std::shared_ptr<Object>& rhs) const {
    return this->operator std::shared_ptr<Object>() == rhs;
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
  SharedPointerSlot& operator=(const SharedPointerSlot& rhs) {
    // Our own stuff is going away, need to reset ourselves
    this->~SharedPointerSlot();

    // Placement construct the right-hand side into ourselves:
    rhs.New(this, sizeof(*this));
    return *this;
  }

  /// <summary>
  /// In-place polymorphic transformer
  /// </summary>
  template<class T>
  SharedPointerSlotT<T>& operator=(const std::shared_ptr<T>& rhs) {
    if (type() == typeid(auto_id<T>)) {
      // We can just use the equivalence operator, no need to make two calls
      *((SharedPointerSlotT<T>*)this) = rhs;
      return *((SharedPointerSlotT<T>*)this);
    }

    // Clear out what we're holding:
    reset();

    // Now we can safely reinitialize:
    static_assert(sizeof(SharedPointerSlotT<T>) == sizeof(*this), "Cannot instantiate a templated shared pointer slot on this type, it's too large to fit here");
    return *new (this) SharedPointerSlotT<T>(rhs);
  }
};

template<class T>
struct SharedPointerSlotT<T, false>:
  SharedPointerSlot
{
  SharedPointerSlotT(const std::shared_ptr<T>& rhs = std::shared_ptr<T>()) {
    static_assert(
      sizeof(std::shared_ptr<T>) == sizeof(m_space),
      "Slot instance is too large to fit in the base type"
    );

    // Make use of our space to make a shared pointer:
    new (m_space) std::shared_ptr<T>(rhs);
  }

  SharedPointerSlotT(const SharedPointerSlotT<T, false>& rhs) {
    new (m_space) std::shared_ptr<T>(rhs.get());
  }

  ~SharedPointerSlotT(void) override {
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

  virtual void* ptr(void) override {
    // At this level, because this type is runtime, we can only provide runtime detection of misuse
    if(std::is_const<T>::value)
      throw autowiring_error("Attempted to obtain a non-const void pointer value from a const-type shared pointer");
    return (void*)get().get();
  }
  virtual const void* ptr(void) const override {
    return (const void*)get().get();
  }

  virtual void New(void* pSpace, size_t nBytes) const override {
    if(nBytes < sizeof(*this))
      throw std::runtime_error("Attempted to construct a SharedPointerSlotT in a space that was too small");
    new (pSpace) SharedPointerSlotT<T, false>(*this);
  }

  bool empty(void) const { return get() == nullptr; }
  const std::type_info& type(void) const override { return typeid(auto_id<T>); }

  void reset(void) override {
    get().reset();
  }

  template<class U>
  bool operator==(const std::shared_ptr<U>& rhs) const {
    return get() == rhs;
  }

  // We have a better opeartor overload for type T:
  void operator=(const std::shared_ptr<T>& rhs) {
    get() = rhs;
  }

  T* operator->(void) const {
    return get().get();
  }
};

template<class T>
struct SharedPointerSlotT<T, true>:
  SharedPointerSlotT<T, false>
{
  SharedPointerSlotT(void) {}

  SharedPointerSlotT(const std::shared_ptr<T>& rhs) :
    SharedPointerSlotT<T, false>(rhs)
  {}

  SharedPointerSlotT(const SharedPointerSlotT<T, true>& rhs) :
    SharedPointerSlotT<T, false>(rhs)
  {}

  SharedPointerSlotT(SharedPointerSlotT<T>&& rhs) :
    SharedPointerSlotT<T>(std::move(rhs))
  {}

  virtual void New(void* pSpace, size_t nBytes) const override {
    if (nBytes < sizeof(*this))
      throw std::runtime_error("Attempted to construct a SharedPointerSlotT in a space that was too small");
    new (pSpace) SharedPointerSlotT<T, true>(*this);
  }

  bool try_assign(const std::shared_ptr<Object>& rhs) override {
    // Just perform a dynamic cast:
    auto casted = autowiring::fast_pointer_cast_blind<T, Object>::cast(rhs);
    if (!casted)
      return false;

    SharedPointerSlotT<T, false>::get() = casted;
    return true;
  }

  virtual operator std::shared_ptr<Object>(void) const override {
    return autowiring::fast_pointer_cast_blind<Object, T>::cast(SharedPointerSlotT<T, false>::get());
  }

  using SharedPointerSlotT<T, false>::operator=;
  using SharedPointerSlotT<T, false>::operator==;
};