// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "atomic_object.h"

/// <summary>
/// Enables an automatic self-update when a packet is decorated with the object type.
/// Provides the prior object (the last decorated instance) to all subsequent packets.
/// </summary>
/// <remarks>
/// In order to ensure that this method will be consistent with any other AutoFilter calls,
/// the object inherits from atomic_object, which implements basic locking functionality.
/// </remarks>
template<class object_type>
class AutoSelfUpdate:
public atomic_object<object_type> {
public:
  typedef atomic_object<object_type> atomic;
  typedef typename atomic::object object;
  typedef typename atomic::lock lock;
  typedef typename atomic::unlock unlock;
  typedef typename atomic::shared shared;

  /// \internal
  /// <summary>
  /// The type assigned to the prior value of the object
  /// in order to distinguish it from the AutoFilter argument.
  /// </summary>
  class prior_object: public object {
  public:
    using object::operator =;
  };

  AutoSelfUpdate(void) {
    // Instanties a BasedAutoFilter for the AutoGather method
    *m_factory += [this] (prior_object& prior) {
      std::lock_guard<lock> lock_this(this->m_lock);
      prior = this->m_object;
    };

    *m_factory += [this](const object& update) {
      *this = update;
    };
  }

  using atomic::operator=;

  operator object(void) {
    // NOTE: This avoids "using" keyword with a cast operator overload
    return *(atomic*)this;
  }

protected:
  AutoRequired<AutoPacketFactory> m_factory;
};
