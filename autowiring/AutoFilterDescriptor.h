// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#pragma once
#include "AnySharedPointer.h"
#include "AutoPacket.h"
#include "auto_arg.h"
#include "CallExtractor.h"
#include "Decompose.h"
#include "has_autofilter.h"
#include "is_shared_ptr.h"
#include MEMORY_HEADER
#include FUNCTIONAL_HEADER
#include STL_UNORDERED_SET
#include STL_UNORDERED_MAP

class AutoPacket;
class Deferred;

/// <summary>
/// AutoFilter argument disposition
/// </summary>
struct AutoFilterDescriptorInput {
  AutoFilterDescriptorInput(void) :
    is_input(false),
    is_output(false),
    is_shared(false),
    ti(nullptr)
  {}

  template<class T>
  AutoFilterDescriptorInput(auto_arg<T>&& traits) :
    is_input(auto_arg<T>::is_input),
    is_output(auto_arg<T>::is_output),
    is_shared(auto_arg<T>::is_shared),
    ti(&typeid(typename auto_arg<T>::id_type))
  {}

  const bool is_input;
  const bool is_output;
  const bool is_shared;
  const std::type_info* const ti;

  operator bool(void) const {
    return !!ti;
  }

  template<class T>
  struct rebind {
    operator AutoFilterDescriptorInput() {
      return auto_arg<T>();
    }
  };
};

/// <summary>
/// The unbound part of an AutoFilter, includes everything except the AnySharedPointer
/// </summary>
struct AutoFilterDescriptorStub {
  AutoFilterDescriptorStub(void) :
    m_pType(nullptr),
    m_pArgs(nullptr),
    m_deferred(false),
    m_arity(0),
    m_requiredCount(0),
    m_pCall(nullptr)
  {}

  AutoFilterDescriptorStub(const AutoFilterDescriptorStub& rhs) :
    m_pType(rhs.m_pType),
    m_pArgs(rhs.m_pArgs),
    m_deferred(rhs.m_deferred),
    m_arity(rhs.m_arity),
    m_requiredCount(rhs.m_requiredCount),
    m_pCall(rhs.m_pCall)
  {}

  /// <summary>
  /// Constructs a new packet subscriber entry based on the specified call extractor and call pointer
  /// </summary>
  /// <remarks>
  /// The caller is responsible for decomposing the desired routine into the target AutoFilter call.  The extractor
  /// is required to carry information about the type of the proper member function to be called; t_extractedCall is
  /// required to be instantiated by the caller and point to the AutoFilter proxy routine.
  /// </summary>
  AutoFilterDescriptorStub(const std::type_info* pType, const AutoFilterDescriptorInput* pArgs, bool deferred, t_extractedCall pCall) :
    m_pType(pType),
    m_pArgs(pArgs),
    m_deferred(deferred),
    m_arity(0),
    m_requiredCount(0),
    m_pCall(pCall)
  {
    for(auto pArg = m_pArgs; *pArg; pArg++) {
      m_arity++;

      if (pArg->is_input)
        ++m_requiredCount;
    }
  }

protected:
  // Type of the subscriber itself
  const std::type_info* m_pType;

  // This subscriber's argument types
  // NOTE: This is a reference to a static generated list,
  // therefore it MUST be const and MUST be shallow-copied.
  const AutoFilterDescriptorInput* m_pArgs;

  // Set if this is a deferred subscriber.  Deferred subscribers cannot receive immediate-style
  // decorations, and have additional handling considerations when dealing with non-copyable
  // decorations.
  bool m_deferred;

  // The number of parameters that will be extracted from the repository object when making
  // a Call.  This is used to prime the AutoPacket in order to make saturation checking work
  // correctly.
  size_t m_arity;

  // The number of arguments declared to be required:
  size_t m_requiredCount;

  // The first argument of this static global is void*, but it is expected that the argument
  // that will actually be passed is of a type corresponding to the member function bound
  // by this operation.  Strong guarantees must be made that the types passed into this routine
  // are identical to the types expected by the corresponding call.
  t_extractedCall m_pCall;

public:
  // Accessor methods:
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

  AutoFilterDescriptor(const AutoFilterDescriptor& rhs) :
    AutoFilterDescriptorStub(rhs),
    m_autoFilter(rhs.m_autoFilter)
  {}

  AutoFilterDescriptor& operator=(const AutoFilterDescriptor& rhs) {
    AutoFilterDescriptorStub::operator = (rhs);
    m_autoFilter = rhs.m_autoFilter;
    return *this;
  }

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
      Decompose<decltype(&T::AutoFilter)>::template Enumerate<AutoFilterDescriptorInput>::types,
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
  AutoFilterDescriptor(Fn fn):
    AutoFilterDescriptor(
      AnySharedPointer(std::make_shared<Fn>(std::forward<Fn>(fn))),
      &typeid(Fn),
      CallExtractor<decltype(&Fn::operator())>::template Enumerate<AutoFilterDescriptorInput>::types,
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
  AutoFilterDescriptor(const AnySharedPointer& autoFilter, const std::type_info* pType, const AutoFilterDescriptorInput* pArgs, bool deferred, t_extractedCall pCall) :
    AutoFilterDescriptorStub(pType, pArgs, deferred, pCall),
    m_autoFilter(autoFilter)
  {}

  template<class RetType, class... Args>
  AutoFilterDescriptor(RetType(*pfn)(Args...)):
    AutoFilterDescriptor(
      // Token shared pointer, used to provide a pointer to pfn because we can't
      // capture it in a template processing context.  Hopefully this can be changed
      // once MSVC adopts constexpr.
      AnySharedPointer(
        std::shared_ptr<RetType(Args...)>(
          pfn,
          [](decltype(pfn)){}
        )
      ),

      // The remainder is fairly straightforward
      CallExtractor<decltype(pfn)>(),
      &CallExtractor<decltype(pfn)>::Call
    )
  {}

  // Convenience overload:
  template<class RetType, class... Args>
  AutoFilterDescriptor(RetType(&pfn)(Args...)):
    AutoFilterDescriptor(&pfn)
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
  public std::true_type
{
public:
  AutoFilterDescriptorSelect(const std::shared_ptr<T>& subscriber) :
    desc(subscriber)
  {}

  const AutoFilterDescriptor desc;
};

template<class T>
class AutoFilterDescriptorSelect<T, false>:
  public std::false_type
{
public:
  AutoFilterDescriptorSelect(const std::shared_ptr<T>&) {}
  const AutoFilterDescriptor desc;
};

/// <summary>
/// Utility routine to support the creation of an AutoFilterDescriptor from any member function
/// </summary>
template<class T>
AutoFilterDescriptor MakeAutoFilterDescriptor(const std::shared_ptr<T>& ptr) {
  return std::move(AutoFilterDescriptorSelect<T>(ptr).desc);
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
