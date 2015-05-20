// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "altitude.h"
#include "auto_arg.h"
#include "AutoFilterDescriptorInput.h"
#include "CallExtractor.h"
#include "Decompose.h"
#include "has_autofilter.h"
#include "is_shared_ptr.h"
#include MEMORY_HEADER

class AutoPacket;
class Deferred;

/// <summary>
/// The unbound part of an AutoFilter, includes everything except the AnySharedPointer representing the filter proper
/// </summary>
struct AutoFilterDescriptorStub {
  AutoFilterDescriptorStub(void) = default;
  AutoFilterDescriptorStub(const AutoFilterDescriptorStub&) = default;
  AutoFilterDescriptorStub& operator=(const AutoFilterDescriptorStub&) = default;

  /// <summary>
  /// Constructs a new packet subscriber entry based on the specified call extractor and call pointer
  /// </summary>
  /// <param name="pType">The type of the underlying filter</param>
  /// <param name="pArgs">The inputs accepted by the filter</param>
  /// <param name="deferred">True if the filter is deferred</param>
  /// <param name="pCall">A pointer to the AutoFilter call routine itself</param>
  /// <remarks>
  /// The caller is responsible for decomposing the desired routine into the target AutoFilter call.  The extractor
  /// is required to carry information about the type of the proper member function to be called; t_extractedCall is
  /// required to be instantiated by the caller and point to the AutoFilter proxy routine.
  /// </summary>
  AutoFilterDescriptorStub(const std::type_info* pType, autowiring::altitude altitude, const AutoFilterDescriptorInput* pArgs, bool deferred, t_extractedCall pCall) :
    m_pType(pType),
    m_altitude(altitude),
    m_pArgs(pArgs),
    m_deferred(deferred),
    m_arity(0),
    m_requiredCount(0),
    m_pCall(pCall)
  {
    for(auto pArg = m_pArgs; *pArg; pArg++) {
      m_arity++;
      
      // time shifted arguments arn't required
      if (pArg->is_input)
        ++m_requiredCount;
    }
  }

protected:
  // Type of the subscriber itself
  const std::type_info* m_pType = nullptr;

  // Altitude--controls when the filter gets called
  autowiring::altitude m_altitude = autowiring::altitude::Standard;

  // This subscriber's argument types
  // NOTE: This is a reference to a static generated list,
  // therefore it MUST be const and MUST be shallow-copied.
  const AutoFilterDescriptorInput* m_pArgs = nullptr;

  // Set if this is a deferred subscriber.  Deferred subscribers cannot receive immediate-style
  // decorations, and have additional handling considerations when dealing with non-copyable
  // decorations.
  bool m_deferred = false;

  // The number of parameters that will be extracted from the repository object when making
  // a Call.  This is used to prime the AutoPacket in order to make saturation checking work
  // correctly.
  size_t m_arity = 0;

  // The number of arguments declared to be required:
  size_t m_requiredCount = 0;

  // The first argument of this static global is void*, but it is expected that the argument
  // that will actually be passed is of a type corresponding to the member function bound
  // by this operation.  Strong guarantees must be made that the types passed into this routine
  // are identical to the types expected by the corresponding call.
  t_extractedCall m_pCall = nullptr;

public:
  // Accessor methods:
  autowiring::altitude GetAltitude(void) const { return m_altitude; }
  const std::type_info* GetType() const { return m_pType; }
  size_t GetArity(void) const { return m_arity; }
  size_t GetRequiredCount(void) const { return m_requiredCount; }
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

  /// <returns>A call lambda wrapping the associated subscriber</returns>
  /// <remarks>
  /// Parameters for the associated subscriber are obtained by querying the packet.
  /// The packet must already be decorated with all required parameters for the
  /// subscribers, or an exception will be thrown.
  /// </remarks>
  t_extractedCall GetCall(void) const { return m_pCall; }
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
  AutoFilterDescriptor(const AutoFilterDescriptor&) = default;
  AutoFilterDescriptor& operator=(const AutoFilterDescriptor&) = default;
  AutoFilterDescriptor(AutoFilterDescriptor&& rhs) :
    AutoFilterDescriptorStub(std::move(rhs)),
    m_autoFilter(std::move(rhs.m_autoFilter))
  {}

  AutoFilterDescriptor& operator=(AutoFilterDescriptor&& rhs) {
    AutoFilterDescriptorStub::operator = (std::move(rhs));
    m_autoFilter = std::move(rhs.m_autoFilter);
    return *this;
  }

  /// <summary>
  /// Utility constructor, used when there is no proffered AutoFilter method on a class
  /// </summary>
  AutoFilterDescriptor(const AnySharedPointer& autoFilter):
    m_autoFilter(autoFilter)
  {}

  template<class T>
  AutoFilterDescriptor(const std::shared_ptr<T>& subscriber) :
    AutoFilterDescriptor(
      AnySharedPointer(
        // Because T::AutoFilter might actually be present in a _base type_ of T, it's important that
        // AnySharedPointer be instantiated with a pointer to the base type where the AutoFilter is
        // actually defined.  In order to obtain this name, we decompose this member function, and
        // then take the type of the decomposed result.
        std::static_pointer_cast<typename Decompose<decltype(&T::AutoFilter)>::type>(subscriber)
      ),
      &typeid(T),
      autowiring::altitude_of<
        T,
        CallExtractor<decltype(&T::AutoFilter)>::deferred ? autowiring::altitude::Dispatch : autowiring::altitude::Standard
      >::value,
      Decompose<decltype(&T::AutoFilter)>::template Enumerate<AutoFilterDescriptorInput, AutoFilterDescriptorInputT>::types,
      CallExtractor<decltype(&T::AutoFilter)>::deferred,
      &CallExtractor<decltype(&T::AutoFilter)>::template Call<&T::AutoFilter>
    )
  {}

  /// <summary>
  /// Adds a function to be called as an AutoFilter for this packet only.
  /// </summary>
  /// <remarks>
  /// Recipients added in this way cannot receive piped data, since they are anonymous.
  /// </remarks>
  template<class Fn>
  AutoFilterDescriptor(Fn fn, autowiring::altitude altitude = autowiring::altitude::Standard):
    AutoFilterDescriptor(
      AnySharedPointer(std::make_shared<Fn>(std::forward<Fn>(fn))),
      &typeid(Fn),
      altitude,
      CallExtractor<decltype(&Fn::operator())>::template Enumerate<AutoFilterDescriptorInput, AutoFilterDescriptorInputT>::types,
      false,
      &CallExtractor<decltype(&Fn::operator())>::template Call<&Fn::operator()>
    )
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
  AutoFilterDescriptor(const AnySharedPointer& autoFilter, const std::type_info* pType, autowiring::altitude altitude, const AutoFilterDescriptorInput* pArgs, bool deferred, t_extractedCall pCall) :
    AutoFilterDescriptorStub(pType, altitude, pArgs, deferred, pCall),
    m_autoFilter(autoFilter)
  {}

  template<class RetType, class... Args>
  AutoFilterDescriptor(RetType(*pfn)(Args...), autowiring::altitude altitude = autowiring::altitude::Standard) :
    AutoFilterDescriptor(
      // Token shared pointer, used to provide a pointer to pfn because we can't
      // capture it in a template processing context.  Hopefully this can be changed
      // once MSVC adopts constexpr.
      AnySharedPointer(
        std::shared_ptr<void>(
          (void*)pfn,
          [](void*){}
        )
      ),

      // The remainder is fairly straightforward
      &typeid(pfn),
      altitude,
      CallExtractor<decltype(pfn)>::template Enumerate<AutoFilterDescriptorInput, AutoFilterDescriptorInputT>::types,
      false,
      CallExtractor<decltype(pfn)>::Call
    )
  {}

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
    return
      std::tie(m_altitude, m_pCall, m_autoFilter) <
      std::tie(rhs.m_altitude, rhs.m_pCall, rhs.m_autoFilter);
  }

  // Operator overloads:
  bool operator<=(const AutoFilterDescriptor& rhs) const { return *this < rhs || *this == rhs;}
  bool operator>(const AutoFilterDescriptor& rhs) const { return !(*this <= rhs);}
  bool operator>=(const AutoFilterDescriptor& rhs) const { return !(*this < rhs);}
  bool operator!=(const AutoFilterDescriptor& rhs) const { return !(*this == rhs); }
};

/// <summary>
/// Utility routine to support the creation of an AutoFilterDescriptor from T::AutoFilter
/// </summary>
/// <remarks>
/// This method will return an empty descriptor in the case that T::AutoFilter is not defined
/// </remarks>
template<class T>
AutoFilterDescriptor MakeAutoFilterDescriptor(const std::shared_ptr<T>& ptr) {
  return MakeAutoFilterDescriptor(ptr, std::integral_constant<bool, has_autofilter<T>::value>());
}

/// <summary>
/// Alias for AutoFilterDescriptor(ptr)
/// </summary>
template<class T>
AutoFilterDescriptor MakeAutoFilterDescriptor(const std::shared_ptr<T>& ptr, std::true_type) {
  return AutoFilterDescriptor(ptr);
}

/// <summary>
/// Utility routine to support the creation of an AutoFilterDescriptor from T::AutoFilter
/// </summary>
/// <remarks>
/// This method will return an empty descriptor in the case that T::AutoFilter is not defined
/// </remarks>
template<class T>
AutoFilterDescriptor MakeAutoFilterDescriptor(const std::shared_ptr<T>&, std::false_type) {
  return AutoFilterDescriptor();
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
