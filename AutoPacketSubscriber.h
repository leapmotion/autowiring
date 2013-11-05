#pragma once
#include "Decompose.h"
#include <boost/any.hpp>

class AutoPacket;

/// <summary>
/// Subscriber wrap, represents a single logical subscriber
/// </summary>
/// <remarks>
/// A logical subscriber is any autowired member of the current context which implements
/// a nonstatic member function called AutoFilter that accepts one or more types.  This
/// function is invoked via a call centralizer implemented in the Decompose header, and
/// instantiated from the templatized version of this class's constructor.
/// </remarks>
class AutoPacketSubscriber {
public:
  // The type of the call centralizer
  typedef void(*t_call)(void*, const AutoPacket&);

  AutoPacketSubscriber(void) :
    m_ti(nullptr),
    m_arity(0),
    m_pObj(nullptr),
    m_pCall(nullptr)
  {}

  AutoPacketSubscriber(const AutoPacketSubscriber& rhs) :
    m_subscriber(rhs.m_subscriber),
    m_ti(rhs.m_ti),
    m_arity(rhs.m_arity),
    m_pObj(rhs.m_pObj),
    m_pCall(rhs.m_pCall)
  {}

  /// <summary>
  /// Constructs a new packet subscriber entry based on the specified subscriber
  /// </summary>
  /// <remarks>
  /// This constructor increments the reference count on the passed object until the
  /// object is freed.  A subscriber wraps the templated type, automatically mapping
  /// desired arguments into the correct locations, via use of Decompose::Call and
  /// a AutoPacket to provide type sources
  /// </summary>
  template<class T>
  AutoPacketSubscriber(std::shared_ptr<T> subscriber) :
    m_subscriber(subscriber),
    m_ti(&typeid(T)),
    m_pObj(subscriber.get())
  {
    typedef Decompose<decltype(&T::AutoFilter)> t_decompose;
    CallExtractor<T, std::is_same<Deferred, typename t_decompose::retType>::value> e;

    m_arity = t_decompose::N;
    m_pCall = e();
  }

protected:
  // A hold on the enclosed subscriber
  boost::any m_subscriber;

  // The type information of this entry, used for profiling
  const std::type_info* m_ti;

  // The number of parameters that will be extracted from the repository object when making
  // a Call.  This is used to prime the AutoPacket in order to make saturation checking work
  // correctly.
  size_t m_arity;

  // This is the first argument that will be passed to the Call function defined below.  It
  // is a pointer to an actual subscriber, but with type information omitted for performance.
  void* m_pObj;

  // The first argument of this static global is void*, but it is expected that the argument
  // that will actually be passed is of a type corresponding to the member function bound
  // by this operation.  Strong guarantees must be made that the types
  t_call m_pCall;

public:
  // Accessor methods:
  bool empty(void) const { return m_subscriber.empty(); }
  size_t GetArity(void) const { return m_arity; }
  boost::any GetSubscriber(void) const { return m_subscriber; }
  const std::type_info* GetSubscriberTypeInfo(void) const { return m_ti; }

  /// <summary>
  /// Releases the bound subscriber and the corresponding arity, causing it to become disabled
  /// </summary>
  void ReleaseSubscriber(void) {
    m_arity = 0;
    m_subscriber = boost::any();
  }

  /// <returns>A pointer to the subscriber</returns>
  void* GetSubscriberPtr(void) const { return m_pObj; }

  /// <returns>A call lambda wrapping the associated subscriber</returns>
  /// <remarks>
  /// Parameters for the associated subscriber are obtained by querying the packet.
  /// The packet must already be decorated with all required parameters for the
  /// subscribers, or an exception will be thrown.
  /// </remarks>
  t_call GetCall(void) const { return m_pCall; }
};