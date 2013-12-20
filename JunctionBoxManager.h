#pragma once
#include "DispatchQueue.h"
#include "EventDispatcher.h"
#include "EventReceiver.h"
#include "LockFreeList.h"
#include "LockReducedCollection.h"
#include "SharedPtrHash.h"
#include "TransientPoolBase.h"
#include <boost/thread/shared_mutex.hpp>
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER
#include SHARED_PTR_HEADER
#include STL_UNORDERED_SET
#include TYPE_TRAITS_HEADER
#include <set>

class JunctionBoxManager{
public:
  JunctionBoxManager();
  virtual ~JunctionBoxManager();
};
