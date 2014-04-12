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
  SharedPointerSlot(void) {}

  template<class T>
  SharedPointerSlot(const std::shared_ptr<T>& rhs) {
    // Delegate the remainder to the assign operation:
    *this = rhs;
  }

  // Base destructor, doesn't do anything because there's nothing to release from
  // the base class
  virtual ~SharedPointerSlot(void) {
    auto& type = this->type();

    if(type != typeid(void))
      // VFT assurance.  If the slot was stack-allocated, then the compiler will not call
      // the correct dtor; we need to ensure that this happens anyway.
      ((SharedPointerSlot*)this)->~SharedPointerSlot();
  }

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
  virtual operator void*(void) const {
    return nullptr;
  }

  /// <returns>
  /// True if this slot holds nothing
  /// </returns>
  bool empty(void) const { return type() == typeid(void); }

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
  std::shared_ptr<T>& as(void) {
    if(type() != typeid(T))
      throw std::runtime_error("Attempted to obtain a shared pointer for an unrelated type");

    return ((SharedPointerSlot<T>*)this)->get();
  }

  /// <summary>
  /// In-place polymorphic transformer
  /// </summary>
  template<class T>
  SharedPointerSlotT<T>& operator=(const std::shared_ptr<T>& rhs) {
    if(type() == typeid(T))
      // We can just use the equivalence operator, no need to make two calls
      return *((SharedPointerSlotT<T>*)this) = rhs;

    if(!empty())
      // We aren't presently empty, we need to release our
      // current implementation before attempting to reinitialize
      this->~SharedPointerSlot();

    // Now we can safely reinitialize:
    return *new (this) SharedPointerSlotT<T>(rhs);
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
    // Our own stuff is going away, need to return here
    reset();

    // If the right-hand side is empty, we'll just reset ourselves and be done with it:
    if(rhs.empty())
      return *this;

    // We want to scrape the VFT, which always appears prior to the data block.  We need
    // to use pointer magic to find the address of the VFT.  On most systems it's just
    // one void* space at the top of the class, but we can't be too careful here.
    size_t headerSpace = offsetof(SharedPointerSlot, m_space);
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
    return *this;
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

  virtual operator void*(void) const { return get().get(); }
  const std::type_info& type(void) const override { return typeid(T); }

  // We have a better opeartor overload for type T:
  SharedPointerSlotT& operator=(const std::shared_ptr<T>& rhs) {
    get() = rhs;
    return *this;
  }
};

struct AnySharedPointer {
public:
  AnySharedPointer(void) {
    new (m_space) SharedPointerSlot;
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
  operator void*(void) const {
    return slot().operator void*();
  }

  /// <returns>
  /// True if this slot holds nothing
  /// </returns>
  bool empty(void) const { return slot().empty(); }

  /// <returns>
  /// True if this pointer slot holds an instance of the specified type
  /// </returns>
  template<class T>
  bool is(void) const { return slot().is<T>(); }

  /// <returns>
  /// Returns the type of the shared pointer held in this slot, or typeid(void) if empty
  /// </returns>
  const std::type_info& type(void) const { return slot().type(); }

  /// <summary>
  /// Clears this type, if a shared pointer is currently held
  /// </summary>
  void reset(void) {
    slot().reset();
  }

  /// <summary>
  /// Attempts to coerce this type to the speceified type
  /// </summary>
  template<class T>
  std::shared_ptr<T>& as(void) {
    return slot().as<T>();
  }

  /// <summary>
  /// In-place polymorphic transformer
  /// </summary>
  template<class T>
  SharedPointerSlotT<T>& operator=(const std::shared_ptr<T>& rhs) {
    return slot() = rhs;
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
  AnySharedPointer& operator=(const AnySharedPointer& rhs) {
    *(SharedPointerSlot*) m_space = rhs.slot();
    return *this;
  }
};

static_assert(!std::is_polymorphic<AnySharedPointer>::value, "The shared pointer cannot be polymorphic");
