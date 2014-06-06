#pragma once
#include "AnySharedPointer.h"
#include "AutoPacket.h"
#include "auto_out.h"
#include "Decompose.h"
#include "has_autofilter.h"
#include "optional_ptr.h"
#include <functional>
#include MEMORY_HEADER

class AutoPacket;
class Deferred;

template<class T>
class auto_pooled;

enum eSubscriberInputType {
  // Unused type, refers to an unrecognized input
  inTypeInvalid,

  // Specifies that this argument is mandatory for the AutoFilter to be called
  inTypeRequired,

  // Specifies that this argument is optional--the filter generally may be called
  // any time all required arguments are satisfied, no matter how many optional
  // arguments remain
  inTypeOptional,

  // Specifies that the argument is an output which must be satisfied manually by
  // the caller
  outTypeRef,

  // Specifies that the argument will automatically be marked ready, unless explicitly
  // cancelled by the caller.
  outTypeRefAutoReady
};

/// <summary>
/// Default behavior, which is to just obtain an input for the specified type
/// </summary>
template<class T>
struct subscriber_traits {
  static_assert(
    std::is_const<typename std::remove_reference<T>::type>::value ||
    !std::is_reference<T>::value,
    "If a decoration is desired, it must either be a const reference, or by value"
  );

  typedef T type;
  static const eSubscriberInputType subscriberType = inTypeRequired;

  const T& operator()(AutoPacket& packet) const {
    return packet.Get<T>();
  }
};

/// <summary>
/// Output behavior for the single specified type
/// </summary>
template<class T>
struct subscriber_traits<T&> {
  typedef T type;
  static const eSubscriberInputType subscriberType = outTypeRef;

  AutoCheckout<T> operator()(AutoPacket& packet) const {
    // Inputs by reference are automatically and unconditionally ready:
    AutoCheckout<T> rv = packet.Checkout<T>();
    rv.Ready();
    return rv;
  }
};

/// <summary>
/// Mandatory input type
/// </summary>
template<class T>
struct subscriber_traits<const T&>:
  subscriber_traits<T>
{};

/// <summary>
/// Optional input type
/// </summary>
template<class T>
struct subscriber_traits<optional_ptr<T>> {
  typedef T type;
  static const eSubscriberInputType subscriberType = inTypeOptional;

  // Optional pointer overload, tries to satisfy but doesn't throw if there's a miss
  optional_ptr<T> operator()(AutoPacket& packet) const {
    const typename std::decay<T>::type* out;
    if(packet.Get(out))
      return out;
    return nullptr;
  }
};

template<class T, bool auto_ready>
struct subscriber_traits<auto_out<T, auto_ready>> {
  typedef T type;
  static const eSubscriberInputType subscriberType = auto_ready ? outTypeRef : outTypeRefAutoReady;

  auto_out<T, auto_ready> operator()(AutoPacket& packet) const {
    return auto_out<T, auto_ready>(packet.Checkout<T>());
  }
};

template<>
struct subscriber_traits<AutoPacket&> {
  typedef AutoPacket type;
  static const eSubscriberInputType subscriberType = inTypeRequired;

  AutoPacket& operator()(AutoPacket& packet) const {
    return packet;
  }
};

/// <summary>
/// Specialization for immediate mode cases
/// </summary>
template<class MemFn>
struct CallExtractor;

template<class T, class... Args>
struct CallExtractor<void (T::*)(Args...)>:
  Decompose<void (T::*)(Args...)>
{
  static const bool deferred = false;
  static const size_t N = sizeof...(Args);

  /// <summary>
  /// Binder struct, lets us refer to an instance of Call by type
  /// </summary>
  template<void(T::*memFn)(Args...)>
  static void Call(void* pObj, AutoPacket& autoPacket) {
    // Handoff
    (((T*) pObj)->*memFn)(
      subscriber_traits<Args>()(autoPacket)...
    );
  }
};

/// <summary>
/// Specialization for deferred cases
/// </summary>
template<class T, class... Args>
struct CallExtractor<Deferred (T::*)(Args...)>:
  Decompose<void (T::*)(Args...)>
{
  static const bool deferred = true;
  static const size_t N = sizeof...(Args);

  template<Deferred(T::*memFn)(Args...)>
  static void Call(void* pObj, AutoPacket& autoPacket) {
    // Obtain a shared pointer of the AutoPacket in order to ensure the packet
    // stays resident when we pend this lambda to the destination object's
    // dispatch queue.
    auto pAutoPacket = autoPacket.shared_from_this();

    // Pend the call to this object's dispatch queue:
    *(T*) pObj += [pObj, pAutoPacket] {
      (((T*) pObj)->*memFn)(
        subscriber_traits<Args>()(*pAutoPacket)...
      );
    };
  }
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
/// The unbound part of an AutoFilter, includes everything except the AnySharedPointer
/// </summary>
struct AutoFilterDescriptorStub {
  // The type of the call centralizer
  typedef void(*t_call)(void*, AutoPacket&);

  AutoFilterDescriptorStub(void) :
    m_pType(nullptr),
    m_pArgs(nullptr),
    m_deferred(false),
    m_arity(0),
    m_requiredCount(0),
    m_optionalCount(0),
    m_pCall(nullptr)
  {}

  AutoFilterDescriptorStub(const AutoFilterDescriptorStub& rhs) :
    m_pType(rhs.m_pType),
    m_pArgs(rhs.m_pArgs),
    m_deferred(rhs.m_deferred),
    m_arity(rhs.m_arity),
    m_requiredCount(rhs.m_requiredCount),
    m_optionalCount(rhs.m_optionalCount),
    m_pCall(rhs.m_pCall)
  {}

  /// <summary>
  /// Constructs a new packet subscriber entry based on the specified call extractor and call pointer
  /// </summary>
  /// <remarks>
  /// The caller is responsible for decomposing the desired routine into the target AutoFilter call.  The extractor
  /// is required to carry information about the type of the proper member function to be called; t_call is required
  /// to be instantiated by the caller and point to the AutoFilter proxy routine.
  /// </summary>
  template<class MemFn>
  AutoFilterDescriptorStub(CallExtractor<MemFn> extractor, t_call pCall) :
    m_pType(&typeid(typename Decompose<MemFn>::type)),
    m_requiredCount(0),
    m_optionalCount(0),
    m_arity(extractor.N),
    m_pArgs(extractor.template Enumerate<AutoFilterDescriptorInput>()),
    m_pCall(pCall)
  {
    // Cannot register a subscriber with zero arguments:
    static_assert(CallExtractor<MemFn>::N, "Cannot register a subscriber whose AutoFilter method is arity zero");

    m_deferred = extractor.deferred;
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
  // Type of the subscriber itself
  const std::type_info* m_pType;

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
  size_t GetArity(void) const { return m_arity; }
  size_t GetRequiredCount(void) const { return m_requiredCount; }
  size_t GetOptionalCount(void) const { return m_optionalCount; }
  const AutoFilterDescriptorInput* GetAutoFilterInput(void) const { return m_pArgs; }
  bool IsDeferred(void) const { return m_deferred; }
  const std::type_info* GetAutoFilterTypeInfo(void) const { return m_pType; }

  /// <returns>A call lambda wrapping the associated subscriber</returns>
  /// <remarks>
  /// Parameters for the associated subscriber are obtained by querying the packet.
  /// The packet must already be decorated with all required parameters for the
  /// subscribers, or an exception will be thrown.
  /// </remarks>
  t_call GetCall(void) const { return m_pCall; }
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
struct AutoFilterDescriptor:
  AutoFilterDescriptorStub
{
  AutoFilterDescriptor(void) {}

  AutoFilterDescriptor(const AutoFilterDescriptor& rhs) :
    AutoFilterDescriptorStub(rhs),
    m_autoFilter(rhs.m_autoFilter)
  {}

  /// <summary>
  /// Utility constructor, used when there is no proffered AutoFilter method on a class
  /// </summary>
  AutoFilterDescriptor(const AnySharedPointer& autoFilter):
    m_autoFilter(autoFilter)
  {}

  /// <summary>
  /// Alternative constructor which can bind a stub
  /// </summary>
  AutoFilterDescriptor(const AnySharedPointer& autoFilter, const AutoFilterDescriptorStub& stub) :
    AutoFilterDescriptorStub(stub),
    m_autoFilter(autoFilter)
  {}

  /// <summary>
  /// Constructs a new packet subscriber entry based on the specified subscriber
  /// </summary>
  /// <remarks>
  /// This constructor increments the reference count on the passed object until the object is freed.  A
  /// subscriber wraps the templated type, automatically mapping desired arguments into the correct locations,
  /// via use of Decompose::Call and a AutoPacket to provide type sources
  ///
  /// The caller is responsible for decomposing the desired routine into the target AutoFilter call
  /// </summary>
  template<class MemFn>
  AutoFilterDescriptor(const AnySharedPointer& autoFilter, CallExtractor<MemFn> extractor, t_call pCall) :
    AutoFilterDescriptorStub(extractor, pCall),
    m_autoFilter(autoFilter)
  {
    // Cannot register a subscriber with zero arguments:
    static_assert(CallExtractor<MemFn>::N, "Cannot register a subscriber whose AutoFilter method is arity zero");
  }

protected:
  // A hold on the enclosed autoFilter
  AnySharedPointer m_autoFilter;

public:
  // Accessor methods:
  bool empty(void) const { return !m_pCall || m_autoFilter->empty(); }
  AnySharedPointer& GetAutoFilter(void) { return m_autoFilter; }
  const AnySharedPointer& GetAutoFilter(void) const { return m_autoFilter; }

  /// <summary>
  /// Releases the bound subscriber and the corresponding arity, causing it to become disabled
  /// </summary>
  void ReleaseAutoFilter(void) {
    m_arity = 0;
    m_autoFilter->reset();
  }

  /// <returns>
  /// True if this subscriber instance is empty
  /// </returns>
  operator bool(void) const { return !empty(); }

  bool operator==(const AutoFilterDescriptor& rhs) const {
    return
      m_pCall == rhs.m_pCall &&
      m_autoFilter == rhs.m_autoFilter;
  }
};

template<class T, bool has_autofilter = has_autofilter<T>::value>
class AutoFilterDescriptorSelect:
  public std::true_type,
  public AutoFilterDescriptor
{
public:
  AutoFilterDescriptorSelect(const std::shared_ptr<T>& subscriber) :
    AutoFilterDescriptor(
      subscriber,
      CallExtractor<decltype(&T::AutoFilter)>(),
      &CallExtractor<decltype(&T::AutoFilter)>::template Call<&T::AutoFilter>
    )
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

/// <summary>
/// Utility routine to support the creation of an AutoFilterDescriptor from any member function
/// </summary>
template<class T>
AutoFilterDescriptor MakeAutoFilterDescriptor(const std::shared_ptr<T>& ptr) {
  return AutoFilterDescriptorSelect<T>(ptr);
}

namespace std {
  template<>
  struct hash<AutoFilterDescriptor>
  {
    size_t operator()(const AutoFilterDescriptor& subscriber) const {
      return (size_t) subscriber.GetAutoFilter()->ptr();
    }
  };
}