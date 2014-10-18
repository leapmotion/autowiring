// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "JunctionBoxBase.h"
#include "JunctionBoxEntry.h"
#include "uuid.h"
#include <map>
#include <stdexcept>
#include <vector>
#include <cassert>
#include TYPE_INDEX_HEADER
#include MEMORY_HEADER
#include STL_UNORDERED_MAP
#include TYPE_TRAITS_HEADER

class EventOutputStreamBase;
class CoreContext;
class JunctionBoxBase;
class OutputStreamVector;

template<typename T>
class EventOutputStream;

/// <summary>
/// General manager class of all junction boxes defined in some context
/// </summary>
class JunctionBoxManager {
public:
  JunctionBoxManager();
  virtual ~JunctionBoxManager();

  /// <summary>
  /// Get a JunctionBox by type
  /// </summary>
  std::shared_ptr<JunctionBoxBase> Get(const std::type_index& pTypeIndex);

  /// <summary>
  /// Allows this JunctionBox manager to begin processing events
  /// </summary>
  void Initiate(void);

  void AddEventReceiver(JunctionBoxEntry<Object> receiver);
  void RemoveEventReceiver(JunctionBoxEntry<Object> pRecvr);

  /// <summary>
  /// This method checks whether eventoutputstream listeners for the given type still exist.
  /// For a given type in a hash, returns a vector of weak ptrs.
  /// Goes through the weak ptrs, locks them, erases dead ones.
  /// If any live ones found return true. Otherwise false.
  /// NOTE: this func does lazy cleanup on weakptrs ptng to suff that has fallen out of scope.
  /// </summary>
  bool CheckEventOutputStream(const std::type_index& type);

  /// <summary>
  /// Adds the named eventoutputstream to the collection of known eventoutputstreams
  /// </summary>
  void AddEventOutputStream(const std::type_index& type, std::weak_ptr<EventOutputStreamBase> pRecvr);

  /// <summary>
  /// Creates a new event stream based on the provided event type
  /// </summary>
  template<class T>
  std::shared_ptr<EventOutputStream<T>> CreateEventOutputStream(void) {
    static_assert(uuid_of<T>::value, "Cannot create an output stream on type T, the type was not defined with DECLARE_UUID");
    auto retval =  std::make_shared<EventOutputStream<T>>();
    auto upcastptr = static_cast<std::shared_ptr<EventOutputStreamBase>>(retval);
    std::weak_ptr<EventOutputStreamBase> weakStreamPtr = upcastptr;
    AddEventOutputStream(typeid(T), weakStreamPtr);
    return retval;
  }


protected:
  // All EventOutputStreams objects known to this JunctionBoxManager:
  std::map<std::type_index, std::vector<std::weak_ptr<EventOutputStreamBase>>> m_eventOutputStreams;

  // All junction boxes known by this manager:
  typedef std::unordered_map<std::type_index, std::shared_ptr<JunctionBoxBase>> t_junctionBoxes;
  t_junctionBoxes m_junctionBoxes;
};
