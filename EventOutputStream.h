#pragma once
#include "Decompose.h"
#include <map>
#include <memory>
#include <iostream>
#include <sstream>

#ifndef EnableIdentity
#define EnableIdentity(x) SpecialAssign("x", x) 
#endif


class EventOutputStreamBase {
private:
  std::stringstream m_OutputStream;
public:

  EventOutputStreamBase(void);
  virtual ~EventOutputStreamBase(void);

  /// <returns>
  /// True if this stream has no events in it
  /// </returns>
  bool IsEmpty(void) const;

  /// <returns>
  /// The number of bytes currently stored in the output stream
  /// </returns>
  size_t GetSize(void) const;

  /// <returns>
  /// A pointer to the first byte of data stored in this output stream
  /// </returns>
  const void* GetData(void) const;

  /// <summary>
  /// Resets the output stream, preparing it for new writes.  Does not release or reallocate memory.
  /// </summary>
  void Reset(void);
  
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
  
  template <class Memfn, class Arg1>
  //SFINAE STUB OUT: replace with check_if overloads <<
  typename std::enable_if< std::is_same<Arg1, std::basic_string<char> const *>::value, void >::type     
    Serialize(Memfn & memfn, Arg1 & arg1 ){
    m_OutputStream << *arg1;
    std::string outtest;
    m_OutputStream >> outtest; // just here if you wanna get it back
  }
  
  template <class Memfn, class Arg1>
  //SFINAE STUB OUT: replace with check_if overloads <<
  typename std::enable_if<!std::is_same<Arg1, std::basic_string<char> const *>::value, void >::type 
  Serialize(Memfn & memfn, Arg1 & arg1){
    std::cout <<  "Hi! Fall through case does nothing" << std::endl;
    //If the program ever actually executes this code it should fail miserably.
    //That would mean an enable memfn tried to serialize an unserializable arg.
  }

};

/// <summary>
/// An output stream, which represents marshalled output from an event type in a context
/// </summary>
template<class T>
class EventOutputStream:
  public EventOutputStreamBase
{
private:
	typedef void (T::*fnPtr)(std::string); 
    std::map<std::string, fnPtr> m_oneArgsMap;
public:
  EventOutputStream(){}
  ~EventOutputStream(){}
  void New(){}

  /// <summary>
  /// Enables a new event for serialization via its identity
  /// </summary>
  template<class MemFn>
  void SpecialAssign(std::string, MemFn eventIden) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");
	IsEnabled(eventIden, true);
	/*
	Then something to the effect of:
	Check if memfn has been enabled
	Try to add memfn to the m_oneArgsMap. When you serialize, serialize the VALUE
	When you deserialize, deserializer the KEY of the buddy map in EventInputStream.h
	*/
  }


};

