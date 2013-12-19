#pragma once
#include <string>
#include <map>

#ifndef EnableIdentity
#define EnableIdentity(x) SpecialAssign("x", x) 
#endif
/// <summary>
/// Allows the deserialization of events from an output stream, in order to replay them in-process
/// </summary>
template<class T>
class EventInputStream
{
public:
  static_assert(std::is_base_of<EventReceiver, T>::value, "Cannot instantiate an event input stream on a non-event type");

private:
	typedef void (T::*fnPtr)(std::string); 
    std::map<std::string, fnPtr> m_oneArgsMap;
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
  /// Enables a new event for deserialization via its identity
  /// </summary>
  template<class MemFn>
  void SpecialAssign(std::string, MemFn eventIden) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");
    IsEnabled(eventIden, true);
  }

  /// <summary>
  /// Interprets and fires a SINGLE EVENT from the passed input buffer
  /// </summary>
  /// <returns>
  /// The number of bytes processed from the input buffer
  /// </returns>
  size_t FireSingle(const void* pData, size_t dataSize) const {
    return 0;
  }
};

