// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "atomic_object.h"
#include MUTEX_HEADER

template<class object, class lock> class atomic_object;

///<summary>
///A reference to a atomic_object that guarantees both existence and exclusive mutability.
///</summary>
///<remarks>
///An unlock_object cannot be copied by construction or assignment since it maintains access.
///An unlock_object cannot be used to extend the existence of an atomic_object beyond the
///span of existence of the unlock_object.
///</remarks>
template<class object_type, class lock_type = std::mutex>
class unlock_object {
public:
  typedef atomic_object<object_type, lock_type> atomic;
  typedef typename atomic::object object;
  typedef typename atomic::lock lock;
  typedef typename atomic::shared shared;

  unlock_object(const unlock_object<object, lock>& source) = delete;
  unlock_object<object, lock>& operator = (unlock_object<object, lock>& _rhs) = delete;
  unlock_object<object, lock>* operator & () = delete;

protected:
  //CONTRACT: m_shared != nullptr while m_shared->m_lock is held
  shared m_shared;

public:
  ///<summary>
  ///Default constructor, yielding bool(*this) == false;
  ///</summary>
  unlock_object() {}

  ///<summary>
  ///Construction references the object and maintains a lock, yielding bool(*this) == true;
  ///</summary>
  ///<param name="should_try">If true, the returned unlock_object might hold no reference or lock</param>
  unlock_object(shared source, bool should_try = false) {
    acquire(source, should_try);
  }

  ///<summary>
  ///Destruction releases any shared reference and any held lock.
  ///</summary>
  ~unlock_object() {
    release();
  }

  ///<summary>
  ///Releases reference and lock, yielding bool(this) == false;
  ///</summary>
  ///<remarks>
  ///This method is idempotent.
  ///</remarks>
  void release() {
    if (m_shared) {
      m_shared->m_lock.unlock();
      m_shared.reset();
    }
  }

  ///<summary>
  ///Aquires a lock on and reference to source, yielding bool(*this) == true;
  ///</summary>
  ///<param name="should_try">If true, the returned unlock_object might hold no reference or lock</param>
  ///<remarks>
  ///This method is idempotent, including when called repeatedly with the same argument.
  ///However, acquire(source) always releases the any currently held lock.
  ///</remarks>
  void acquire(shared source, bool should_try = false) {
    release();
    if (!source) {
      return;
    }
    if (!should_try) {
      source->m_lock.lock();
    } else if (!source->m_lock.try_lock()) {
      return;
    }
    m_shared = source;
  }

  ///<returns>True when unlock_object references and locks a atomic_object</returns>
  operator bool () {
    return m_shared.operator bool ();
  }

  object& operator * () const {return m_shared->m_object;}
  object* operator -> () const {return &m_shared->m_object;}
};
