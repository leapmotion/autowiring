// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "DataFlow.h"
#include "AutoPacket.h"
#include "auto_out.h"
#include "Decompose.h"
#include "has_autofilter.h"
#include "is_autofilter.h"
#include MEMORY_HEADER
#include FUNCTIONAL_HEADER
#include STL_UNORDERED_SET
#include STL_UNORDERED_MAP

class AutoPacket;
class Deferred;

template<class T>
class auto_pooled;

enum eSubscriberInputType {
  // Unused type, refers to an unrecognized input
  inTypeInvalid,

  // Required Input ~ const T&
  // Specifies that this argument is mandatory for the AutoFilter to be called
  inTypeRequired,

  // Optional Input
  // Specifies that this argument is optional--the filter generally may be called
  // any time all required arguments are satisfied, no matter how many optional
  // arguments remain
  inTypeOptional,

  // Optional Output ~ shared_ptr<T>&
  // Specifies that the argument is an output which must be satisfied manually by
  // the caller
  outTypeRef,

  // Required Output
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
  typedef const T& ret_type;
  static const eSubscriberInputType subscriberType = inTypeRequired;

  const T& operator()(AutoPacket& packet, const std::type_info& source) const {
    return packet.Get<T>(source);
  }
};

/// <summary>
/// Output behavior for the single specified type
/// </summary>
template<class T>
struct subscriber_traits<T&> {
  typedef T type;
  typedef AutoCheckout<T> ret_type;
  static const eSubscriberInputType subscriberType = outTypeRef;

  AutoCheckout<T> operator()(AutoPacket& packet, const std::type_info& source) const {
    // Inputs by reference are automatically and unconditionally ready:
    AutoCheckout<T> rv = packet.Checkout<T>(source);
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
  typedef optional_ptr<T> ret_type;
  static const eSubscriberInputType subscriberType = inTypeOptional;

  // Optional pointer overload, tries to satisfy but doesn't throw if there's a miss
  optional_ptr<T> operator()(AutoPacket& packet, const std::type_info& source) const {
    const typename std::decay<T>::type* out;
    if(packet.Get(out, source))
      return out;
    return nullptr;
  }
};

/// <summary>
/// Output types
/// </summary>
template<class T, bool auto_ready>
struct subscriber_traits<auto_out<T, auto_ready>> {
  typedef T type;
  typedef auto_out<T, auto_ready> ret_type;
  static const eSubscriberInputType subscriberType = auto_ready ? outTypeRef : outTypeRefAutoReady;

  auto_out<T, auto_ready> operator()(AutoPacket& packet, const std::type_info& source) const {
    return auto_out<T, auto_ready>(packet.Checkout<T>(source));
  }
};

/// <summary>
/// Dependent type enabling argument
/// </summary>
template<>
struct subscriber_traits<AutoPacket&> {
  typedef AutoPacket type;
  typedef AutoPacket& ret_type;
  static const eSubscriberInputType subscriberType = inTypeRequired;

  AutoPacket& operator()(AutoPacket& packet, const std::type_info&) const {
    return packet;
  }
};

/// <summary>
/// Provides a means of associating dynamic source types with static argument types
/// </summary>
template<class Arg>
struct sourced_checkout {
  typename subscriber_traits<Arg>::ret_type operator()(AutoPacket& packet, const DataFill& satisfaction) const {
    DataFill::const_iterator source_find = satisfaction.find(typeid(typename subscriber_traits<Arg>::type));
    if (source_find != satisfaction.end()) {
      return subscriber_traits<Arg>()(packet, *source_find->second);
    }
    return subscriber_traits<Arg>()(packet, typeid(void));
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
  static void Call(void* pObj, AutoPacket& autoPacket, const DataFill& satisfaction) {
    // Handoff
    (((T*) pObj)->*memFn)(
      sourced_checkout<Args>()(autoPacket, satisfaction)...
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
  static void Call(void* pObj, AutoPacket& autoPacket, const DataFill& satisfaction) {
    // Obtain a shared pointer of the AutoPacket in order to ensure the packet
    // stays resident when we pend this lambda to the destination object's
    // dispatch queue.
    auto pAutoPacket = autoPacket.shared_from_this();

    // Pend the call to this object's dispatch queue:
    // WARNING: The DataFill information will be referenced,
    // since it should be from a SatCounter associated to autoPacket,
    // and will therefore have the same lifecycle as the AutoPacket.
    *(T*) pObj += [pObj, pAutoPacket, &satisfaction] {
      (((T*) pObj)->*memFn)(
        sourced_checkout<Args>()(*pAutoPacket, satisfaction)...
      );
    };
  }
};

/// <summary>
/// AutoFilter argument disposition
/// </summary>
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

  static bool isInput(eSubscriberInputType argType) {
    return argType == inTypeRequired || argType == inTypeOptional;
  }

  static bool isOutput(eSubscriberInputType argType) {
    return argType == outTypeRef || argType == outTypeRefAutoReady;
  }

  bool isInput() const { return isInput(subscriberType); }

  bool isOutput() const { return isOutput(subscriberType); }
};

/// <summary>
/// The unbound part of an AutoFilter, includes everything except the AnySharedPointer
/// </summary>
struct AutoFilterDescriptorStub {
  // The type of the call centralizer
  typedef void(*t_call)(void*, AutoPacket&, const DataFill&);

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
    m_dataMap(rhs.m_dataMap),
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
    m_pArgs(extractor.template Enumerate<AutoFilterDescriptorInput>()),
    m_deferred(extractor.deferred),
    m_arity(extractor.N),
    m_requiredCount(0),
    m_optionalCount(0),
    m_pCall(pCall)
  {
    // Cannot register a subscriber with zero arguments:
    static_assert(CallExtractor<MemFn>::N, "Cannot register a subscriber whose AutoFilter method is arity zero");

    for(auto pArg = m_pArgs; *pArg; pArg++) {
      // DEFAULT: All data is broadcast
      DataFlow& data = m_dataMap[*pArg->ti];
      data.output = AutoFilterDescriptorInput::isOutput(pArg->subscriberType);
      data.broadcast = true;
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
  // NOTE: This is a reference to a static generated list,
  // therefor it MUST be const and MUST be shallow-copied.
  const AutoFilterDescriptorInput* m_pArgs;
  typedef std::unordered_map<std::type_index, DataFlow> FlowMap;
  FlowMap m_dataMap;

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
  // by this operation.  Strong guarantees must be made that the types passed into this routine
  // are identical to the types expected by the corresponding call.
  t_call m_pCall;

public:
  // Accessor methods:
  size_t GetArity(void) const { return m_arity; }
  size_t GetRequiredCount(void) const { return m_requiredCount; }
  size_t GetOptionalCount(void) const { return m_optionalCount; }
  const AutoFilterDescriptorInput* GetAutoFilterInput(void) const { return m_pArgs; }
  bool IsDeferred(void) const { return m_deferred; }
  const std::type_info* GetAutoFilterTypeInfo(void) const { return m_pType; }

  /// <summary>
  /// Orientation (input/output, required/optional) of the argument type.
  /// </summary>
  /// <remarks>
  /// Returns nullptr when no argument is of the requested type.
  /// </remarks>
  const AutoFilterDescriptorInput* GetArgumentType(const std::type_info* argType) {
    for(auto pArg = m_pArgs; *pArg; pArg++) {
      if (pArg->ti == argType) {
        return pArg;
      }
    }
    return nullptr;
  }

  /// <summary>
  /// Copies the data flow information for the argument type to the flow argument.
  /// </summary>
  /// <returns>true when the argument type is found</returns>
  DataFlow GetDataFlow(const std::type_info* argType) const {
    FlowMap::const_iterator data = m_dataMap.find(*argType);
    if (data != m_dataMap.end()) {
      return data->second;
    }
    return DataFlow(); //DEFAULT: No flow
  }

  /// <returns>A call lambda wrapping the associated subscriber</returns>
  /// <remarks>
  /// Parameters for the associated subscriber are obtained by querying the packet.
  /// The packet must already be decorated with all required parameters for the
  /// subscribers, or an exception will be thrown.
  /// </remarks>
  t_call GetCall(void) const { return m_pCall; }

  /// <summary>
  /// Sends or receives broadcast instances of the input or output type.
  /// </summary>
  /// <remarks>
  /// The dataType must declared by the AutoFilter method for this call to have an effect.
  /// </remarks>
  /// <param="dataType">specifies the data type (input or output) to broadcast</param>
  /// <param="enable">when false disables broadcasting</param>
  void Broadcast(const std::type_info* dataType, bool enable = true) {
    FlowMap::iterator flowFind = m_dataMap.find(*dataType);
    if (flowFind == m_dataMap.end())
      return;
    DataFlow& flow = flowFind->second;
    flow.broadcast = enable;
  }

  /// <summary>
  /// Creates a data half-pipe from this node to the target node for the specifed data.
  /// </summary>
  /// <remarks>
  /// A complete pipe requires that both the input and output nodes are modified.
  /// This method only modifies this node - the other half-pipe requires a call to the other node.
  /// The dataType must declared by the AutoFilter method for this call to have an effect.
  /// </remarks>
  /// <param="dataType">specifies the data type (input or output) to pipe</param>
  /// <param="nodeType">determines the target node that will receive the data</param>
  /// <param="enable">when false removes a pipe, if it exists</param>
  void HalfPipe(const std::type_info* dataType, const std::type_info* nodeType, bool enable = true) {
    FlowMap::iterator flowFind = m_dataMap.find(*dataType);
    if (flowFind == m_dataMap.end())
      return;
    DataFlow& flow = flowFind->second;
    if (enable)
      flow.halfpipes.insert(*nodeType);
    else
      flow.halfpipes.erase(*nodeType);
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
  /// via use of Decompose::Call and a AutoPacket to provide type satisfaction
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
  /// True if this subscriber instance is not empty.
  /// </returns>
  operator bool(void) const { return !empty(); }

  /// <returns>True when both the AutoFilter method and subscriber instance are equal.</returns>
  bool operator==(const AutoFilterDescriptor& rhs) const {
    // AutoFilter methods are the same for all instances of a class,
    // and classes may have more than one AutoFilter method,
    // so both comparisons are required.
    return
      m_pCall == rhs.m_pCall &&
      m_autoFilter == rhs.m_autoFilter;
  }

  /// <summary>
  /// Default for std library sorting of unique elements
  /// </summary>
  bool operator<(const AutoFilterDescriptor& rhs) const {
    if (m_pCall < rhs.m_pCall)
      return true;
    return m_autoFilter < rhs.m_autoFilter;
  }

  /// <summary>
  /// Default for std library sorting of repeatable elements
  /// </summary>
  bool operator<=(const AutoFilterDescriptor& rhs) const { return *this < rhs || *this == rhs;}

  bool operator>(const AutoFilterDescriptor& rhs) const { return !(*this <= rhs);}

  bool operator>=(const AutoFilterDescriptor& rhs) const { return !(*this < rhs);}
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
