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
  // All EventOutputStreams objects known to this JunctionBoxManager:
  std::map<std::type_index, std::vector<std::weak_ptr<EventOutputStreamBase> > > m_eventOutputStreams;

public:
  
  JunctionBoxManager();
  virtual ~JunctionBoxManager();
  
  std::shared_ptr<JunctionBoxBase> Get(std::type_index);
  void AddEventReceiver(std::shared_ptr<EventReceiver> pRecvr);
  void RemoveEventReceiver(std::shared_ptr<EventReceiver> pRecvr);
  void RemoveEventReceivers(t_rcvrSet::iterator first, t_rcvrSet::iterator last);

  /// <summary>
  /// This method checks whether eventoutputstream listeners for the given type still exist.
  /// For a given type in a hash, returns a vector of weak ptrs.
  /// Goes through the weak ptrs, locks them, erases dead ones.
  /// If any live ones found return true. Otherwise false.
  /// NOTE: this func does lazy cleanup on weakptrs ptng to suff that has fallen out of scope.
  /// </summary>
  template <class T>
  bool CheckEventOutputStream(void){

    auto mapfinditerator= m_eventOutputStreams.find(typeid(T));
    if (mapfinditerator != m_eventOutputStreams.end()){
    auto v = (mapfinditerator->second);
    auto it = v.begin();
    while(it != v.end() ){
    if( (*it).lock() ) return true;
    it = v.erase(it);
    }
    return false; //return false if iterated through whole vec without seeing any live pointers.
    }
    return false;  //return false if no vec with that type
  }

  /// <summary>
  /// Adds the named eventoutputstream to the collection of known eventoutputstreams
  /// </summary>
  template <class T>
  void AddEventOutputStream(std::weak_ptr<EventOutputStreamBase> pRecvr){
    auto mapfinditerator = m_eventOutputStreams.find(typeid(T));
    if (mapfinditerator != m_eventOutputStreams.end()){
      //if the type exists already, find the correspoonding outputstreambase and push it back.
      (mapfinditerator->second).push_back(pRecvr);
    }
    else {
      std::vector<std::weak_ptr<EventOutputStreamBase> > newvec;
      newvec.push_back(pRecvr);
      m_eventOutputStreams[typeid(T)] = newvec; //assignment copy constructor invoked; 
    }
  }

  /// <summary>
  /// Creates a new event stream based on the provided event type
  /// </summary>
  template<class T>
  std::shared_ptr<EventOutputStream<T>> CreateEventOutputStream(void) {
    static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot create an output stream based on a non-event type");
    static_assert(uuid_of<T>::value, "Cannot create an output stream on type T, the type was not defined with DECLARE_UUID");
    auto retval =  std::make_shared<EventOutputStream<T>>();
    auto upcastptr = static_cast<std::shared_ptr<EventOutputStreamBase>>(retval);
    std::weak_ptr<EventOutputStreamBase> weakStreamPtr = upcastptr;
    AddEventOutputStream<T>(weakStreamPtr);
    return retval;   
  }

  
protected:
  t_junctionBoxes m_junctionBoxes;
};
