// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "atomic_object.h"
#include MEMORY_HEADER

///<summary>
///This class implements the atomic_object interface to a shared reference.
///This will enable the construction of unlock_objects that combine the existential
///guarantee of a shared_ptr with the immutability guarantee of a lock_guard.
///</summary>
///<remarks>
///This class guarantees the existence of a referenced atomic_object.
///In order for shared_object to be thread-safe the reference is defined on construction.
///Defining assignment to apply to the referenced object instead of the reference
///ensures that the behavior of a shared_object is identical to an atomic_object,
///since the referenced object cannot be changed.
///</remarks>
template<class object, class lock = std::mutex>
class shared_object {
  friend class unlock_object<object, lock>;

protected:
  //CONTRACT: m_share == true
  std::shared_ptr<atomic_object<object, lock>> m_share;

  lock& get_lock() const {
    return m_share->m_lock;
  }
  object& get_object() {
    return m_share->m_object;
  }
  const object& get_object() const {
    return m_share->m_object;
  }
  bool& get_initialized() {
    return m_share->m_initialized;
  }
  const bool& get_initialized() const {
    return m_share->m_initialized;
  }

public:
  typedef unlock_object<object, lock> unlock;

  ///<summary>
  ///Default constructor instantiates an uninitialized atomic_object.
  ///</summary>
  shared_object():
  m_share(new atomic_object<object, lock>()) {}

  ///<summary>
  ///Copy constructor references the same atomic_object as source.
  ///</summary>
  ///<remarks>
  ///To copy the atomic_object referenced by source use:
  /// shared_object<object> target(*unlock_object<object>(source));
  ///This method does NOT copy the atomic_object referenced by source.
  ///</remarks>
  shared_object(const shared_object<object, lock>& source):
  m_share(source.m_share) {}

  ///<summary>
  ///Copy constructor instantiates a atomic_object initialized by source.
  ///</summary>
  shared_object(const object& source):
  m_share(new atomic_object<object, lock>(source)) {}


  ///<summary>
  ///Assignment to referenced atomic_object from source referenced atomic_object.
  ///</summary>
  ///<remarks>
  ///This method is equivalent to:
  /// target = *unlock_object<object>(source);
  ///This method does NOT redirect the atomic_object reference to the reference of source.
  ///</remarks>
  shared_object<object, lock>& operator = (const shared_object<object, lock>& source) {
    *m_share = *source.m_share;
    return *this;
  }

  ///<summary>
  ///Assignment to current atomic_object.
  ///</summary>
  shared_object<object, lock>& operator = (const object& source) {
    *m_share = source;
    return *this;
  }

  ///<summary>
  ///Cast by copying current state
  ///</summary>
  operator object() const {
    return *m_share;
  }

  ///<summary>
  ///Reset using default constructor yielding initialized() == false.
  ///</summary>
  ///<return>True if the object was not assigned default values</return>
  bool reset() {
    return m_share->reset();
  }

  ///<return>True if the object was not assigned default values</return>
  bool initialized() const {
    return m_share->initialized();
  }

  ///<summary>
  ///Atomic copy of target to this object, only if initialized() == false.
  ///</summary>
  ///<return>True if the object was not assigned default values</return>
  bool reset(const object& target) {
    return m_share->reset(target);
  }

  ///<summary>
  ///Atomic copy of this object to target, only if initialized() == true.
  ///</summary>
  ///<return>True if the object was not assigned default values</return>
  bool initialized(object& target) {
    return m_share->initialized(target);
  }

  ///<summary>
  ///If uninitialized uses target for initialization.
  ///If initialized assigns current value to target.
  ///</summary>
  ///<returns> Returns +1 for transfer from target to this, -1 for transfer from this to target</returns>
  int transfer(object& target) {
    return m_share->transfer(target);
  }

  ///<summary>
  ///If neither this nor target are uninitialized, no transfer occurs.
  ///If this is uninitialized and target is not, then this is initialized by target.
  ///If target is uninitialized and this is, then target is initialized by this.
  ///If both this and target are initialized, no transforer occurs.
  ///</summary>
  ///<returns> Returns +1 for transfer from target to this, -1 for transfer from this to target, else 0</returns>
  int transfer(atomic_object<object, lock>& target) {
    return m_share->transfer(target);
  }

  ///<summary>
  ///If neither this nor target are uninitialized, no transfer occurs.
  ///If this is uninitialized and target is not, then this is initialized by target.
  ///If target is uninitialized and this is, then target is initialized by this.
  ///If both this and target are initialized, no transforer occurs.
  ///</summary>
  ///<returns> Returns +1 for transfer from target to this, -1 for transfer from this to target, else 0</returns>
  int transfer(shared_object<object, lock>& target) {
    return m_share->transfer(target.m_share);
  }
};
