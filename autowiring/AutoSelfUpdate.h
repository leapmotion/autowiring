// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "NewAutoFilter.h"
#include "shared_object.h"

///<summary>
///Enables an automatic self-update when a packet is decorated with the object type.
///Provides the prior object (the last decorated instance) to all subsequent packets.
///</summary>
///<remarks>
///In order to ensure that this method will be consistent with any other AutoFilter calls,
///the object inherits from shared_object, which implements basic locking functionality.
///</remarks>
template<class object, class lock = std::mutex>
class AutoSelfUpdate:
public shared_object<object, lock> {
protected:
  using shared_object<object, lock>::get_lock;
  using shared_object<object, lock>::get_object;

public:
  AutoSelfUpdate() {}
  AutoSelfUpdate(const shared_object<object, lock>& source) : shared_object<object, lock>(source) {}
  AutoSelfUpdate(const object& source) : shared_object<object, lock>(source) {}
  using shared_object<object, lock>::operator =;
  using shared_object<object, lock>::operator object;

  //The distinct type assigned to the prior value of the object
  class prior_object: public object {
  public:
    prior_object(const object& source): object(source) {}
  };

  //Avoid intermediate copy by defining an explicit cast
  operator prior_object() const {
    std::lock_guard<lock> lock_this(get_lock());
    return prior_object(get_object());
  }

  //Decorates all packets with instances of prior_object
  void AutoFilter(AutoPacket& packet) {
    packet.Decorate(this->operator prior_object());
  }

  //Updates this object
  void AutoGather(const object& update) {
    shared_object<object, lock>::operator = (update);
  }

  NewAutoFilter<decltype(&AutoSelfUpdate<object>::AutoGather), &AutoSelfUpdate<object>::AutoGather> SelfUpdate;
};
