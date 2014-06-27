// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "stdafx.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include "shared_object.h"

///<summary>
///A reference to a shared_object that guarantees both existence and exclusive mutability.
///The construction of a guard_object establishes atomic_object initialization, since
///the expectation is that the referenced object will be modified.
///</summary>
///<remarks>
///Unlike a shared_object a guard_object does not guarantee the existence
///of a referenced object. This enables the definition of shared_object::try_unlock().
///A guard_object is not copiable since it maintains access, and cannot be used to
///instantiate new shared_object instances, since it is not guaranteed to reference
///a shared_object.
///</remarks>
template<class object>
class unlock_object {
  friend class shared_object<object>;

  std::shared_ptr<atomic_object<object>> m_share;

  unlock_object(unlock_object<object>& _arg) = delete;
  unlock_object<object>& operator = (unlock_object<object>& _rhs) = delete;

public:
  ///<summary>
  ///Default constructor, yielding unlock_object<object>::operator bool == false;
  ///</summary>
  unlock_object() {}

  ///<summary>
  ///Construction from shared_object references the object and maintains a lock.
  ///</summary>
  ///<remarks>
  ///When _try = true the returned unlock_object might hold nor reference or lock.
  ///</remarks>
  explicit unlock_object(shared_object<object>& _arg, bool _try = false) {
    if (_try &&
        !_arg.m_share->m_lock.try_lock()) {
      return;
    } else {
      _arg.m_share->m_lock.lock();
    }
    _arg.m_share->m_initialized = true;
    m_share = _arg.m_share;
  }

  ///<summary>
  ///Destruction releases any shared reference and any held lock.
  ///</summary>
  ~unlock_object() {
    if (m_share) {
      m_share->m_lock.unlock();
    }
  }

  ///<summary>
  ///Reset releases reference and lock, yielding bool(this) == false;
  ///</summary>
  ///<remarks>
  ///This method is idempotent.
  ///</remarks>
  void reset() {
    if (m_share) {
      m_share->m_lock.unlock();
      m_share.reset();
    }
  }

  ///<summary>
  ///Reset re-targets reference and lock to _arg, yielding bool(this) == false;
  ///</summary>
  ///<remarks>
  ///This method is idempotent, including when called repeatedly with the same argument.
  ///However, reset(_arg) always releases the any currently held lock.
  ///</remarks>
  void reset(shared_object<object>& _arg) {
    if (m_share)
      reset();
    _arg.m_share->m_lock.lock();
    _arg.m_share->m_initialized = true;
    m_share = _arg.m_share;
  }

  ///<returns>True when guard_object references and locks a shared_object</returns>
  operator bool () {return m_share.operator bool ();}

  object& operator * () const {return m_share->m_object;}
  object* operator -> () const {return &m_share->m_object;}
};
