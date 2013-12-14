#pragma once
#include "Decompose.h"

class EventOutputStreamBase {
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
  ~EventOutputStream(){
    std::cout<< "Event outputstream destructor was called" << std::endl;
    /*
    auto toremove = static_cast<std::shared_ptr<EventOutputStreamBase>>(this);//make sure you downcast first. Throw this into base destructorkk
    AutoCurrentContext ctxt;
    ctxt -> RemoveEventOutputStream<T>(toremove);
    */
    std::cout<< "Event outputstream destructor was called" << std::endl;
  }
  void New(){}
  /// <summary>
  /// Enables a new event for serialization via its identity
  /// </summary>
  template<class MemFn>
  void EnableIdentity(MemFn eventIden) {
    // We cannot serialize an identity we don't recognize
    static_assert(std::is_same<typename Decompose<MemFn>::type, T>::value, "Cannot add a member function unrelated to the output type for this class");
  }
};

