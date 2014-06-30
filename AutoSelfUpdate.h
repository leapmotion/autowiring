#pragma once
#include "Autowiring/NewAutoFilter.h"
#include "Autowiring/shared_object.h"
#include "Autowiring/unlock_object.h"

///<summary>
///Enables an automatic self-update when a packet is decorated with the object type.
///</summary>
///<remarks>
///In order to ensure that this method will be consistent with any other AutoFilter calls,
///the object inherits from atomic_object, which implements basic locking functionality.
///</remarks>
template<class object, class lock = boost::mutex>
class AutoSelfUpdate:
public shared_object<object, lock> {
public:

  using shared_object<object, lock>::operator =;
  using shared_object<object, lock>::operator object;

  void SelfUpdate(object& update) {
    shared_object<object, lock>::operator = (update);
  }

  NewAutoFilter<decltype(&AutoSelfUpdate<object>::SelfUpdate), &AutoSelfUpdate<object>::SelfUpdate> AutoFilter_SelfUpdate;
};
