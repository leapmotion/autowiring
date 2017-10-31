// Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.
#pragma once
#include "auto_id.h"
#include "TypeUnifier.h"
#include <atomic>
#include <typeinfo>
#include MEMORY_HEADER

namespace autowiring {

class DeferrableAutowiring;

/// <summary>
/// Represents information about a single slot detected as having been declared in a context member
/// </summary>
struct SlotInformation {
  SlotInformation(const SlotInformation* pFlink, auto_id type, size_t slotOffset, bool autoRequired) :
    pFlink(pFlink),
    type(type),
    slotOffset(slotOffset),
    autoRequired(autoRequired)
  {}

  // The next slot defined on this type
  const SlotInformation* const pFlink;

  // The type of this slot:
  const auto_id type;

  // The offset of this slot relative to the base of the enclosing object
  size_t slotOffset;

  // True if this slot was AutoRequired
  bool autoRequired;
};

/// <summary>
/// Stump entry, used to anchor a chain of slot information entries
/// </summary>
struct SlotInformationStumpBase {
  SlotInformationStumpBase(const std::type_info& ti) :
    ti(ti)
  {
    pHead = nullptr;
  }

  // RTTI to which this stump pertains
  const std::type_info& ti;

  // Initialization flag, used to indicate that this stump has valid data
  bool bInitialized = false;

  // Current slot information:
  std::atomic<const SlotInformation*> pHead;
};

/// <summary>
/// Utility type, inherits true_type if T should use the type unifier
/// </summary>
template<class T>
struct use_unifier:
  std::integral_constant<
    bool,
    std::is_class<T>::value &&
    !std::is_base_of<CoreObject, T>::value
  >
{};

template<class T, bool needsUnifier = use_unifier<T>::value>
struct SlotInformationStump;

template<class T>
struct SlotInformationStump<T, false>:
  SlotInformationStumpBase
{
  SlotInformationStump(void):
    SlotInformationStumpBase(typeid(T))
  {}

  static SlotInformationStump s_stump;
};

/// <summary>
/// Specializations for types which will be using the type unifier
/// </summary>
template<class T>
struct SlotInformationStump<T, true>:
  public SlotInformationStump<typename SelectTypeUnifier<T>::type>
{
};

template<class T>
SlotInformationStump<T, false> SlotInformationStump<T, false>::s_stump;

/// <summary>
/// A stack location linked list, stored in a per-thread basis and used to track slots
/// </summary>
class SlotInformationStackLocation {
public:
  /// <summary>
  /// Registers a new stack location on the current thread, used to provide slot reflection services in Autowiring
  /// </summary>
  SlotInformationStackLocation(SlotInformationStumpBase& stump, const void* pObj = nullptr, size_t extent = 0);

  ~SlotInformationStackLocation(void);

  // The pointer location where the stump will be stored:
  SlotInformationStumpBase& stump;

  // Information about the object being constructed while this stack location is valid:
  const void* const pObj;
  const size_t extent;

  // The prior stack location, made current when this slot goes out of scope
  SlotInformationStackLocation& prior;

private:
  // Current slot information:
  SlotInformation* m_pCur = nullptr;

public:
  /// <returns>
  /// True if the passed pointer is inside of the object currently under construction at this stack location
  /// </returns>
  bool Encloses(const void* ptr) const {
    return pObj < ptr && ptr < (char*) pObj + extent;
  }

  /// <returns>
  /// The offset of the specified pointer from the beginning of the object under construction
  /// </returns>
  /// <param name="ptr">The object whose offset is to be found</param>
  /// <remarks>
  /// The return value is undefined if the passed pointer is not within the object under construction.  This
  /// function is intend to be used during field construction to find the offset of the passed field within
  /// its enclosing type.
  /// </remarks>
  size_t Offset(const void* ptr) const {
    return (char*) ptr - (char*) pObj;
  }

  /// <summary>
  /// Returns the current information stump entry, or null if no stack location exists
  /// </summary>
  static SlotInformationStackLocation* CurrentStackLocation(void);

  /// <summary>
  /// Registers the named slot with the current stack location
  /// </summary>
  static void RegisterSlot(DeferrableAutowiring* pAutowiring);
};

}
