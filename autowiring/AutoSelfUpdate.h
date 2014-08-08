#pragma once
#include "NewAutoFilter.h"
#include "atomic_object.h"

///<summary>
///Enables an automatic self-update when a packet is decorated with the object type.
///Provides the prior object (the last decorated instance) to all subsequent packets.
///</summary>
///<remarks>
///In order to ensure that this method will be consistent with any other AutoFilter calls,
///the object inherits from atomic_object, which implements basic locking functionality.
///</remarks>
template<class object_type, class lock_type = std::mutex>
class AutoSelfUpdate:
public atomic_object<object_type, lock_type> {
public:
  typedef atomic_object<object_type, lock_type> atomic;
  typedef typename atomic::object object;
  typedef typename atomic::lock lock;
  typedef typename atomic::unlock unlock;
  typedef typename atomic::shared shared;

  AutoSelfUpdate() {}
  AutoSelfUpdate(const atomic_object<object, lock>& source) : atomic_object<object, lock>(source) {}
  AutoSelfUpdate(const object& source) : atomic_object<object, lock>(source) {}
  using atomic_object<object, lock>::operator =;
  using atomic_object<object, lock>::operator object;

  //The distinct type assigned to the prior value of the object
  class prior_object: public object {
  public:
    prior_object(const object& source): object(source) {}
  };

  //Avoid intermediate copy by defining an explicit cast
  operator prior_object() const {
    std::lock_guard<lock> lock_this(atomic_object<object, lock>::m_lock);
    return prior_object(atomic_object<object, lock>::m_object);
  }

  //Decorates all packets with instances of prior_object
  void AutoFilter(AutoPacket& packet) {
    packet.Decorate(this->operator prior_object());
  }

  //Updates this object
  void AutoGather(const object& update) {
    atomic_object<object, lock>::operator = (update);
  }

  NewAutoFilter<decltype(&AutoSelfUpdate<object>::AutoGather), &AutoSelfUpdate<object>::AutoGather> SelfUpdate;
};
