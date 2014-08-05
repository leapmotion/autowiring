// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "Decompose.h"
#include "Deserialize.h"
#include <map>
#include <string>
#include <sstream>
#include <cassert>

#include MEMORY_HEADER
#include TYPE_TRAITS_HEADER

#ifndef EnableIdentity
#define EnableIdentity(x) SpecialAssign<decltype(x), x> (#x) 
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
  /// Converts member functions to their string representation for serialization.
  /// </summary>
  template <class MemFn>
  std::string AddAndQueryMemFn(MemFn memfn, std::string str = "Query")
  {
    std::map<std::string, MemFn> local;
    static std::map<std::string, MemFn> my_map = local;
    if(str == "Query")
    {
      for(auto it = my_map.begin(); it != my_map.end(); ++it)
      {
        if((it->second) == memfn) return it->first;
      }
      return "";
    }
    my_map[str] = memfn;
    return "";
  }

  template <class Arg>
  //SFINAE STUB OUT: replace with check_if overloads <<
  typename std::enable_if< std::is_same<Arg, std::basic_string<char> const *>::value, void >::type
    SerializeMethod(Arg & arg){
      m_OutputStream << "\xD8" << *arg;
    }
  
  template <class Arg>
  //stub out such that if ARGUMENT defines a static method called AutoSerialize which returns an std::string,
  typename std::enable_if<std::is_base_of<Auto::Serialize, Arg>::value, void >::type
    SerializeMethod(Arg & arg){
      m_OutputStream << "\xD8" << arg.AutoSerialize();
    }    
  
  template <class Arg1>
  //SFINAE STUB OUT: replace with check_if overloads <<
  typename std::enable_if<!std::is_same<Arg1, std::basic_string<char> const *>::value && !std::is_base_of<Auto::Serialize, Arg1>::value, void >::type
    SerializeMethod(Arg1 & arg1){
      assert(false);
      //static_assert(false, "Fundamental belief about serialized argument types violated");
    }
    

  /// <summary>
  /// Comment.
  /// </summary>
  template<class MemFn>
  void EmitHeader(MemFn eventIden){
    m_OutputStream << AddAndQueryMemFn(eventIden);
  }

  /// <summary>
  /// Comment.
  /// </summary>
  void EmitFooter(void){
    m_OutputStream <<  "\xDE";
  }

  /// <summary>
  /// Recursive serialize message: base case
  /// </summary>
  template <typename... Targs>
  void Serialize2(Targs&... args){
  }
  
  /// <summary>
  /// Recursive serialize message: N-args case
  /// </summary>
  template <typename Head, typename... Targs>
  void Serialize2(Head &value, Targs&... args){
    //Emit an arg
    SerializeMethod(value);
    //m_OutputStream << "\xD8" << *value;
    Serialize2(args...);
  }

  template <typename Memfn, typename... Targs>
  void SerializeInit(Memfn memfn, Targs&... args){
    EmitHeader(memfn);
    Serialize2( args...);
    EmitFooter();
  }

  template <typename Memfn, typename Head, typename... Targs>
  void SerializeInit(Memfn memfn, Head &value, Targs&... args){
    EmitHeader(memfn);
    Serialize2(value, args...);
    EmitFooter();
  }


  // See here: http://stackoverflow.com/questions/257288/is-it-possible-to-write-a-c-template-to-check-for-a-functions-existence

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
  template<class MemFn, MemFn eventIden>
  void SpecialAssign(std::string str) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");
    if (!IsEnabled(eventIden))
    {
    IsEnabled(eventIden,  true);
    AddAndQueryMemFn(eventIden, str);
    }
  }
};