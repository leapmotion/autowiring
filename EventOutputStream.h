#pragma once
#include "Decompose.h"
#include <memory>
#include <iostream>
#include <sstream>





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
  void Serialize(Memfn & memfn, Arg1 & arg1){
	  std::cout <<  "Hi! Just tried to serialize this stuff" << std::endl;
	  m_OutputStream << arg1;
	  std::cout << "From my outputs" << m_OutputStream << std::endl;
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

public:
  EventOutputStream(){}
  ~EventOutputStream(){}
  void New(){}

  /// <summary>
  /// Enables a new event for serialization via its identity
  /// </summary>
  template<class MemFn>
  void EnableIdentity(MemFn eventIden) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");
	IsEnabled(eventIden, true);
  }


};

