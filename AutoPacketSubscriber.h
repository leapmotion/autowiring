#pragma once
#include "Decompose.h"
#include "FilterPropertyExtractor.h"
#include <boost/any.hpp>
#include SHARED_PTR_HEADER

class AutoPacket;
class Deferred;

template<class T, bool is_deferred>
struct CallExtractor {
  typedef void(*t_call)(void*, const AutoPacket&);

  t_call operator()() const {
    typedef decltype(&T::AutoFilter) t_fnType;
    return reinterpret_cast<t_call>(
      &BoundCall<AutoPacket, t_fnType, &T::AutoFilter>::Call
      );
  }
};

template<class T>
struct CallExtractor<T, true> {
  typedef void(*t_call)(void*, const AutoPacket&);

  static void CallDeferred(T* pObj, const AutoPacket& repo) {
    const t_call call =
      reinterpret_cast<t_call>(
      &BoundCall<AutoPacket, decltype(&T::AutoFilter), &T::AutoFilter>::Call
      );
    *pObj += [pObj, &repo, call] {
      call(pObj, repo);
    };
  }

  t_call operator()() const {
    return reinterpret_cast<t_call>(&CallDeferred);
  }
};

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
    m_ppArgs(nullptr),
    m_arity(0),
    m_pObj(nullptr),
    m_pCall(nullptr)
  {}

  AutoPacketSubscriber(const AutoPacketSubscriber& rhs) :
    m_subscriber(rhs.m_subscriber),
    m_ti(rhs.m_ti),
    m_ppArgs(rhs.m_ppArgs),
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
  AutoPacketSubscriber(const std::shared_ptr<T>& subscriber) :
    m_subscriber(subscriber),
    m_ti(&typeid(T)),
    m_pObj(subscriber.get())
  {
    typedef Decompose<decltype(&T::AutoFilter)> t_decompose;
    CallExtractor<T, std::is_same<Deferred, typename t_decompose::retType>::value> e;

    // Cannot register a subscriber with zero arguments:
    static_assert(t_decompose::N, "Cannot register a subscriber whose AutoFilter method is arity zero");

    m_arity = t_decompose::N;
    m_ppArgs = t_decompose::Enumerate();
    m_pCall = e();
  }

protected:
  // A hold on the enclosed subscriber
  boost::any m_subscriber;

  // The type information of this subscriber
  const std::type_info* m_ti;

  // This subscriber's argument types
  const std::type_info*const* m_ppArgs;

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
  const std::type_info*const* GetSubscriberArgs(void) const { return m_ppArgs; }

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

template<class T, bool has_autofilter = has_autofilter<T>::value>
class AutoPacketSubscriberSelect:
  public std::true_type,
  public AutoPacketSubscriber
{
public:
  AutoPacketSubscriberSelect(const std::shared_ptr<T>& subscriber) :
    AutoPacketSubscriber(subscriber)
  {}
};

template<class T>
class AutoPacketSubscriberSelect<T, false>:
  public std::false_type
{
public:
  AutoPacketSubscriberSelect(const std::shared_ptr<T>&) {}
};