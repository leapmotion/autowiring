#pragma once
#include "JunctionBox.h"
#include TYPE_INDEX_HEADER
#include FUNCTIONAL_HEADER
#include RVALUE_HEADER
#include SHARED_PTR_HEADER
#include STL_UNORDERED_SET
#include STL_UNORDERED_MAP
#include TYPE_TRAITS_HEADER
#include <set>

//A thread-safe way to share junctionBoxes between peer contexts

class EventReceiver;
class CoreContext;

class JunctionBoxManager{
  typedef std::unordered_map<std::type_index, std::shared_ptr<JunctionBoxBase>> t_junctionBoxes;
  typedef std::unordered_set<std::shared_ptr<EventReceiver>, SharedPtrHash<EventReceiver>> t_rcvrSet;
public:
  
  JunctionBoxManager();
  virtual ~JunctionBoxManager();
  
  std::shared_ptr<JunctionBoxBase> Get(std::type_index);
  void AddEventReceiver(std::shared_ptr<EventReceiver> pRecvr);
  void RemoveEventReceiver(std::shared_ptr<EventReceiver> pRecvr);
  void RemoveEventReceivers(t_rcvrSet::iterator first, t_rcvrSet::iterator last);
  
protected:
  t_junctionBoxes m_junctionBoxes;
};
