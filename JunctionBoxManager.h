#pragma once
#include "JunctionBox.h"
#include "TypeRegistry.h"
#include "uuid.h"
#include TYPE_INDEX_HEADER
#include MEMORY_HEADER
#include STL_UNORDERED_MAP
#include TYPE_TRAITS_HEADER

class EventReceiver;
class CoreContext;

/// <summary>
/// General manager class of all junction boxes defined in some context
/// </summary>
class JunctionBoxManager {
public:
  JunctionBoxManager();
  virtual ~JunctionBoxManager();

  template<typename T>
  std::shared_ptr<JunctionBoxBase> Get(void) {
    const std::type_index& pTypeIndex = typeid(T);

    // Add an utterance of the TypeRegistry so we can add this AutoFired type to our collection
    (void)RegType<T>::r;

    auto box = m_junctionBoxes.find(pTypeIndex);
    assert(box != m_junctionBoxes.end());

    //Check here if any listening marshals might be interested in receiving the fired args
    auto mapfinditerator = m_eventOutputStreams.find(pTypeIndex);
    std::vector<std::weak_ptr<EventOutputStreamBase> > * OutputStreamVector = nullptr;
    if (mapfinditerator != m_eventOutputStreams.end()){
      //no vec on this type yet. So create it, pass it, and wait for it to get filled later
      OutputStreamVector = &(mapfinditerator->second);
    }
    else {
      std::vector<std::weak_ptr<EventOutputStreamBase> > newvec;
      m_eventOutputStreams[pTypeIndex] = newvec; //assignment copy constructor invoked;
      auto it  = m_eventOutputStreams.find(pTypeIndex);
      OutputStreamVector = &(it->second);
    }

    (box->second)->SetPotentialMarshals(OutputStreamVector);
    return box->second;
  }

  /// <summary>
  /// Allows this JunctionBox manager to begin processing events
  /// </summary>
  void Initiate(void);

  void AddEventReceiver(JunctionBoxEntry<EventReceiver> receiver);
  void RemoveEventReceiver(JunctionBoxEntry<EventReceiver> pRecvr);

  /// <summary>
  /// This method checks whether eventoutputstream listeners for the given type still exist.
  /// For a given type in a hash, returns a vector of weak ptrs.
  /// Goes through the weak ptrs, locks them, erases dead ones.
  /// If any live ones found return true. Otherwise false.
  /// NOTE: this func does lazy cleanup on weakptrs ptng to suff that has fallen out of scope.
  /// </summary>
  template <class T>
  bool CheckEventOutputStream(void){
    auto mapfinditerator = m_eventOutputStreams.find(typeid(T));
    if(mapfinditerator != m_eventOutputStreams.end()) {
      auto v = (mapfinditerator->second);
      auto it = v.begin();
      while(it != v.end()) {
        if((*it).lock())
          return true;
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
      // If the type exists already, find the correspoonding outputstreambase and push it back.
      mapfinditerator->second.push_back(pRecvr);
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
  // All EventOutputStreams objects known to this JunctionBoxManager:
  std::map<std::type_index, std::vector<std::weak_ptr<EventOutputStreamBase>>> m_eventOutputStreams;

  // All junction boxes known by this manager:
  typedef std::unordered_map<std::type_index, std::shared_ptr<JunctionBoxBase>> t_junctionBoxes;
  t_junctionBoxes m_junctionBoxes;
};
