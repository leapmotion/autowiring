#pragma once
#include <string>
#include <deque>
#include <map>
#include <typeinfo>
#include "Autowired.h"

#ifndef EnableIdentity
#define EnableIdentity(x) SpecialAssign(#x, x) 
#endif
/// <summary>
/// Allows the deserialization of events from an output stream, in order to replay them in-process
/// </summary>
template<class T>
class EventInputStream
{
public:
  static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot instantiate an event input stream on a non-event type");
  typedef void (T::*fnPtr)(const std::string *);
  typedef Deferred (T::*DeferredfnPtr)(const std::string *);
  std::map<std::string, fnPtr> Fired_map;
  std::map<std::string, DeferredfnPtr> Deferred_map;

private:
  std::map<std::string, fnPtr> m_oneArgsMap;
  std::shared_ptr<EventReceiverProxy<T>> m_receiver;
public:
  EventInputStream(){}
  /// <summary>
  /// Returns true if memfn is enabled, otherwise false.
  /// </summary>
  template<class MemFn>
  bool IsEnabled(MemFn eventIden, bool amIRegistered = false) {
  static int registration = 0; //first time func is checked
    if (amIRegistered && registration < 1){
      registration++;
    }
  return (!!registration);
  }
 
  /// <summary>
  /// Enables a new FIRED event for deserialization via its identity
  /// </summary>
  template<class MemFn>
  typename std::enable_if< std::is_same<typename Decompose<MemFn>::retType, Deferred>::value, void >::type 
  SpecialAssign(std::string str, MemFn eventIden) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");
    if (!IsEnabled(eventIden))
    {
      IsEnabled(eventIden, true);
      Deferred_map[str] = eventIden;
    }
  }

  /// <summary>
  /// Enables a new DEFERRED event for deserialization via its identity
  /// </summary>
  template<class MemFn>
  typename std::enable_if< std::is_same<typename Decompose<MemFn>::retType, void>::value, void >::type  
  SpecialAssign(std::string str, MemFn eventIden) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");

    if (!IsEnabled(eventIden))
    {
      IsEnabled(eventIden, true);
      Fired_map[str] = eventIden;
    }
  }

  /// <summary>
  /// Interprets and fires a SINGLE EVENT from the passed input buffer
  /// </summary>
  /// <returns>
  /// The number of bytes processed from the input buffer
  /// </returns>
  size_t FireSingle(const void* pData, size_t dataSize) const {
    //First wrap all the bytes in a string.
    auto chptr = static_cast <const char *> (pData);
    std::string MyString (chptr);//, dataSize);

    std::size_t location = MyString.find("Þ");
    std::string topevent = MyString.substr(0, location);

    std::deque<std::string> v;
    std::istringstream buf(topevent);

    std::string s;
    while (std::getline(buf, s, 'Ø'))
        v.push_back(s);

    std::string query = v[0];
    const std::string * ARG1 = &v[1];

    AutoFired<T> sender;

    auto find1 = Fired_map.find(query);
    if (find1 != Fired_map.end()) 
    {
      auto eventpred1 = find1 -> second;
      sender(eventpred1)(ARG1);
    }

    auto find2 = Deferred_map.find(query);
    if (find2 != Deferred_map.end())
    {
      auto eventpred2 = find2 -> second;
      sender.Defer(eventpred2)(ARG1);
    }

    return location +1 ;
  }
};

