// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once

#include "stdafx.h"
#include "shared_object.h"

///<summary>
///A reference to a shared_object that guarantees both existence and exclusive mutability.
///The construction of an unlock_object establishes atomic_object initialization, since
///the expectation is that the referenced object will be modified.
///</summary>
///<remarks>
///Unlike a shared_object an unlock_object does not guarantee the existence
///of a referenced object. This enables the definition of shared_object::try_unlock().
///An unlock_object is not copiable since it maintains access, and cannot be used to
///instantiate new shared_object instances, since it is not guaranteed to reference
///a shared_object.
///</remarks>
template<class object, class lock = boost::mutex>
class unlock_object {
  friend class shared_object<object, lock>;

  unlock_object(unlock_object<object, lock>& _arg) = delete;
  unlock_object<object, lock>& operator = (unlock_object<object, lock>& _rhs) = delete;

  std::shared_ptr<atomic_object<object, lock>> m_share;

public:
  ///<summary>
  ///Default constructor, yielding unlock_object<object>::operator bool == false;
  ///</summary>
  unlock_object() {}

  ///<summary>
  ///Construction from a shared_object references the object and maintains a lock.
  ///</summary>
  ///<remarks>
  ///When _try = true the returned unlock_object might hold no reference or lock.
  ///</remarks>
  unlock_object(shared_object<object, lock>& _arg, bool _try = false) {
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
  ///Construction from an atomic_object references the object and maintains a lock.
  ///</summary>
  ///<remarks>
  ///When _try = true the returned unlock_object might hold no reference or lock.
  ///</remarks>
  unlock_object(atomic_object<object, lock>& _arg, bool _try = false) {
    if (_try &&
        !_arg.m_lock.try_lock()) {
      return;
    } else {
      _arg.m_lock.lock();
    }
    _arg.m_initialized = true;
    m_share.reset(&_arg, [](atomic_object<object, lock>*){});
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
  void reset(shared_object<object, lock>& _arg) {
    if (m_share)
      reset();
    _arg.m_share->m_lock.lock();
    _arg.m_share->m_initialized = true;
    m_share = _arg.m_share;
  }

  ///<returns>True when unlock_object references and locks a shared_object</returns>
  operator bool () {return m_share.operator bool ();}

  object& operator * () const {return m_share->m_object;}
  object* operator -> () const {return &m_share->m_object;}
};
