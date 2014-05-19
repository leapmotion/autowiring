#pragma once
#include "AnySharedPointer.h"
#include "AutoPacket.h"
#include "Decompose.h"
#include "FilterPropertyExtractor.h"
#include MEMORY_HEADER

class AutoPacket;
class AutoPacketAdaptor;
class Deferred;

template<class T, bool auto_ready>
class auto_out;

template<class T>
class auto_pooled;

template<class T>
class optional_ptr;

template<class MemFn>
struct CallExtractor;

/// <summary>
/// Specialization for immediate mode cases
/// </summary>
template<class T, class... Args>
struct CallExtractor<void (T::*)(Args...)>
{
  typedef std::false_type deferred;

  static void Call(void* pObj, const AutoPacketAdaptor& repo) {
    ((T*) pObj)->AutoFilter(
      repo.Cast<Args>()...
    );
  }
};

/// <summary>
/// Specialization for deferredcases
/// </summary>
template<class T, class... Args>
struct CallExtractor<Deferred (T::*)(Args...)>
{
  typedef std::true_type deferred;

  static void Call(void* pObj, const AutoPacketAdaptor& repo) {
    // Obtain a shared pointer of the AutoPacket in order to ensure the packet
    // stays resident when we pend this lambda to the destination object's
    // dispatch queue.
    std::shared_ptr<AutoPacket> autoPacket = repo;

    // Pend the call to this object's dispatch queue:
    *(T*) pObj += [pObj, autoPacket, repo] {
      ((T*) pObj)->AutoFilter(
        repo.Cast<Args>()...
      );
    };
  }
};

enum eSubscriberInputType {
  inTypeInvalid,
  inTypeRequired,
  inTypeOptional,
  outTypeRef,
  outTypeRefAutoReady
};

template<class T>
struct subscriber_traits
{
  typedef T type;
  static const eSubscriberInputType subscriberType = inTypeRequired;
};

template<class T>
struct subscriber_traits<optional_ptr<T>>
{
  typedef T type;
  static const eSubscriberInputType subscriberType = inTypeOptional;
};

template<class T, bool auto_ready>
struct subscriber_traits<auto_out<T, auto_ready>>
{
  typedef T type;
  static const eSubscriberInputType subscriberType = auto_ready ? outTypeRef : outTypeRefAutoReady;
};

struct AutoFilterDescriptorInput {
  AutoFilterDescriptorInput(void) :
    ti(nullptr),
    subscriberType(inTypeInvalid)
  {}

  template<class T>
  AutoFilterDescriptorInput(subscriber_traits<T>&& traits) :
    ti(&typeid(typename subscriber_traits<T>::type)),
    subscriberType(subscriber_traits<T>::subscriberType)
  {}

  const std::type_info* const ti;
  const eSubscriberInputType subscriberType;

  operator bool(void) const {
    return !!ti;
  }

  template<class T>
  struct rebind {
    operator AutoFilterDescriptorInput() {
      return subscriber_traits<T>();
    }
  };
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
class AutoFilterDescriptor {
public:
  // The type of the call centralizer
  typedef void(*t_call)(void*, const AutoPacketAdaptor&);

  AutoFilterDescriptor(void) :
    m_ti(nullptr),
    m_pArgs(nullptr),
    m_deferred(false),
    m_arity(0),
    m_requiredCount(0),
    m_optionalCount(0),
    m_pCall(nullptr)
  {}

  AutoFilterDescriptor(const AutoFilterDescriptor& rhs) :
    m_autoFilter(rhs.m_autoFilter),
    m_ti(rhs.m_ti),
    m_pArgs(rhs.m_pArgs),
    m_deferred(rhs.m_deferred),
    m_arity(rhs.m_arity),
    m_requiredCount(rhs.m_requiredCount),
    m_optionalCount(rhs.m_optionalCount),
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
  AutoFilterDescriptor(const std::shared_ptr<T>& autoFilter) :
    m_autoFilter(autoFilter),
    m_ti(&typeid(T)),
    m_requiredCount(0),
    m_optionalCount(0),
    m_pCall(&CallExtractor<decltype(&T::AutoFilter)>::Call)
  {
    typedef Decompose<decltype(&T::AutoFilter)> t_decompose;

    // Cannot register a subscriber with zero arguments:
    static_assert(t_decompose::N, "Cannot register a subscriber whose AutoFilter method is arity zero");

    m_deferred = std::is_same<Deferred, typename t_decompose::retType>::value;
    m_arity = t_decompose::N;
    m_pArgs = t_decompose::template Enumerate<AutoFilterDescriptorInput>();
    for(auto pArg = m_pArgs; *pArg; pArg++) {
      switch(pArg->subscriberType) {
      case inTypeRequired:
        m_requiredCount++;
        break;
      case inTypeOptional:
        m_optionalCount++;
        break;
      default:
        break;
      }
    }
  }

protected:
  // A hold on the enclosed autoFilter
  AnySharedPointer m_autoFilter;

  // The type information of this subscriber
  const std::type_info* m_ti;

  // This subscriber's argument types
  const AutoFilterDescriptorInput* m_pArgs;

  // Set if this is a deferred subscriber.  Deferred subscribers cannot receive immediate-style
  // decorations, and have additional handling considerations when dealing with non-copyable
  // decorations.
  bool m_deferred;

  // The number of parameters that will be extracted from the repository object when making
  // a Call.  This is used to prime the AutoPacket in order to make saturation checking work
  // correctly.
  size_t m_arity;

  // The number of argumetns declared to be required:
  size_t m_requiredCount;

  // The number of arguments declared to be optional:
  size_t m_optionalCount;

  // The first argument of this static global is void*, but it is expected that the argument
  // that will actually be passed is of a type corresponding to the member function bound
  // by this operation.  Strong guarantees must be made that the types
  t_call m_pCall;

public:
  // Accessor methods:
  bool empty(void) const { return m_autoFilter->empty(); }
  size_t GetArity(void) const { return m_arity; }
  size_t GetRequiredCount(void) const { return m_requiredCount; }
  size_t GetOptionalCount(void) const { return m_optionalCount; }
  const AnySharedPointer& GetAutoFilter(void) const { return m_autoFilter; }
  const std::type_info* GetAutoFilterTypeInfo(void) const { return m_ti; }
  const AutoFilterDescriptorInput* GetAutoFilterInput(void) const { return m_pArgs; }
  bool IsDeferred(void) const { return m_deferred; }

  bool operator==(const AutoFilterDescriptor& rhs) const {
    return
      m_pCall == rhs.m_pCall &&
      m_autoFilter == rhs.m_autoFilter;
  }

  /// <summary>
  /// Releases the bound subscriber and the corresponding arity, causing it to become disabled
  /// </summary>
  void ReleaseAutoFilter(void) {
    m_arity = 0;
    m_autoFilter->reset();
  }

  /// <returns>A pointer to the proper subscriber object</returns>
  void* GetAutoFilterPtr(void) { return m_autoFilter->ptr(); }
  const void* GetAutoFilterPtr(void) const { return m_autoFilter->ptr(); }

  /// <returns>A call lambda wrapping the associated subscriber</returns>
  /// <remarks>
  /// Parameters for the associated subscriber are obtained by querying the packet.
  /// The packet must already be decorated with all required parameters for the
  /// subscribers, or an exception will be thrown.
  /// </remarks>
  t_call GetCall(void) const { return m_pCall; }

  /// <returns>
  /// True if this subscriber instance is empty
  /// </returns>
  operator bool(void) const { return !empty(); }
};

template<class T, bool has_autofilter = has_autofilter<T>::value>
class AutoFilterDescriptorSelect:
  public std::true_type,
  public AutoFilterDescriptor
{
public:
  AutoFilterDescriptorSelect(const std::shared_ptr<T>& subscriber) :
    AutoFilterDescriptor(subscriber)
  {}
};

template<class T>
class AutoFilterDescriptorSelect<T, false>:
  public std::false_type,
  public AutoFilterDescriptor
{
public:
  AutoFilterDescriptorSelect(const std::shared_ptr<T>&) {}
};

template<class T>
AutoFilterDescriptor MakeAutoFilterDescriptor(const std::shared_ptr<T>& ptr) {
  return AutoFilterDescriptorSelect<T>(ptr);
}

namespace std {
  template<class T>
  struct hash;

  template<>
  struct hash<AutoFilterDescriptor>:
    public std::unary_function<AutoFilterDescriptor, size_t>
  {
    size_t operator()(const AutoFilterDescriptor& subscriber) const {
      return (size_t) subscriber.GetAutoFilterPtr();
    }
  };
}