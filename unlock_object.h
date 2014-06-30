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
///An unlock_object cannot be copied by construction or assignment since it maintains access.
///An unlock_object and cannot be used to instantiate new shared_object instances,
///because it is not guaranteed to reference a shared_object.
///An unlock_object cannot be applied to an atomic_object, since continued existence of the
///referenced object would not be guaranteed.
///</remarks>
template<class object, class lock = boost::mutex>
class unlock_object {
  friend class shared_object<object, lock>;

  unlock_object(unlock_object<object, lock>& source) = delete;
  unlock_object<object, lock>& operator = (unlock_object<object, lock>& _rhs) = delete;

  std::shared_ptr<atomic_object<object, lock>> m_share;

  void unlock(shared_object<object, lock>& source) {
    source.m_share->m_lock.lock();
    source.m_share->m_initialized = true;
    m_share = source.m_share;
  }

  void try_unlock(shared_object<object, lock>& source) {
    if(!source.m_share->m_lock.try_lock())
      return;
    source.m_share->m_initialized = true;
    m_share = source.m_share;
  }

public:
  ///<summary>
  ///Default constructor, yielding unlock_object<object>::operator bool == false;
  ///</summary>
  unlock_object() {}

  ///<summary>
  ///Construction from a shared_object references the object and maintains a lock.
  ///</summary>
  ///<param name="should_try">If true, the returned unlock_object might hold no reference or lock</param>
  unlock_object(shared_object<object, lock>& source, bool should_try = false) {
    if(should_try)
      try_unlock(source);
    else
      unlock(source);
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
  ///Reset re-targets reference and lock to source, yielding bool(this) == false;
  ///</summary>
  ///<param name="should_try">If true, the returned unlock_object might hold no reference or lock</param>
  ///<remarks>
  ///This method is idempotent, including when called repeatedly with the same argument.
  ///However, reset(source) always releases the any currently held lock.
  ///</remarks>
  void reset(shared_object<object, lock>& source, bool should_try = false) {
    if (m_share)
      reset();
    if(should_try)
      try_unlock(source);
    else
      unlock(source);
  }

  ///<returns>True when unlock_object references and locks a shared_object</returns>
  operator bool () {return m_share.operator bool ();}

  object& operator * () const {return m_share->m_object;}
  object* operator -> () const {return &m_share->m_object;}
};
