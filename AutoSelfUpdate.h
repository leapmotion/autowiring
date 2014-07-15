#pragma once
#include "Autowiring/NewAutoFilter.h"
#include "Autowiring/atomic_object.h"

///<summary>
///Enables an automatic self-update when a packet is decorated with the object type.
///Provides the prior object (the last decorated instance) to all subsequent packets.
///</summary>
///<remarks>
///In order to ensure that this method will be consistent with any other AutoFilter calls,
///the object inherits from atomic_object, which implements basic locking functionality.
///</remarks>
template<class object, class lock = boost::mutex>
class AutoSelfUpdate:
public atomic_object<object, lock> {
public:

  void AutoFilter(AutoPacket& packet) {
    //TODO: Decorate with prior<object>
  }

  void SelfUpdate(object& update) {
    atomic_object<object, lock>::operator = (update);
  }

  NewAutoFilter<decltype(&AutoSelfUpdate<object>::SelfUpdate), &AutoSelfUpdate<object>::SelfUpdate> AutoFilter_SelfUpdate;
};
