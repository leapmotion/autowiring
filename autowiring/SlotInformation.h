// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#pragma once
#include "TypeUnifier.h"
#include <typeinfo>
#include MEMORY_HEADER

struct AutoFilterDescriptorStub;
class DeferrableAutowiring;

struct AutoFilterDescriptorStubLink {
  AutoFilterDescriptorStubLink(const AutoFilterDescriptorStub& stub, const AutoFilterDescriptorStubLink* pFlink) :
    stub(stub),
    pFlink(pFlink)
  {}

  const AutoFilterDescriptorStub& stub;
  const AutoFilterDescriptorStubLink* const pFlink;
};

/// <summary>
/// Represents information about a single slot detected as having been declared in a context member
/// </summary>
struct SlotInformation {
  SlotInformation(const SlotInformation* pFlink, const std::type_info& type, size_t slotOffset, bool autoRequired) :
    pFlink(pFlink),
    type(type),
    slotOffset(slotOffset),
    autoRequired(autoRequired)
  {}

  // The next slot defined on this type
  const SlotInformation* const pFlink;

  // The type of this slot:
  const std::type_info& type;

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
    ti(ti),
    bInitialized(false),
    pHead(nullptr),
    pFirstAutoFilter(nullptr)
  {}

  // RTTI to which this stump pertains
  const std::type_info& ti;

  // Initialization flag, used to indicate that this stump has valid data
  bool bInitialized;

  // Current slot information:
  const SlotInformation* pHead;

  // If there are any custom AutoFilter fields defined, this is the first of them
  // Note that these custom fields -only- include fields registered via the AutoFilter
  // registration type
  const AutoFilterDescriptorStubLink* pFirstAutoFilter;
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
  SlotInformationStackLocation(const SlotInformationStackLocation& rhs) = delete;
  SlotInformationStackLocation(SlotInformationStackLocation&& rhs) = delete;

  /// <summary>
  /// Registers a new stack location on the current thread, used to provide slot reflection services in Autowiring
  /// </summary>
  SlotInformationStackLocation(SlotInformationStumpBase* pStump, const void* pObj = nullptr, size_t extent = 0);

  ~SlotInformationStackLocation(void);

private:
  // The prior stack location, made current when this slot goes out of scope
  SlotInformationStackLocation* m_pPrior;

  // The pointer location where the stump will be stored:
  SlotInformationStumpBase* m_pStump;

  // Current slot information:
  SlotInformation* m_pCur;

  // Most recent AutoFilter descriptor link:
  AutoFilterDescriptorStubLink* m_pLastLink;

  // Information about the object being constructed while this stack location is valid:
  const void* m_pObj;
  size_t m_extent;

public:
  /// <returns>
  /// True if the passed pointer is inside of the object currently under construction at this stack location
  /// </returns>
  bool Encloses(const void* ptr) {
    return m_pObj < ptr && ptr < (char*) m_pObj + m_extent;
  }

  /// <summary>
  /// The slot information stump for this stack location
  /// </summary>
  SlotInformationStumpBase* GetStump(void) const {
    return m_pStump;
  }

  /// <summary>
  /// Returns the current information stump entry, or null if no stack location exists
  /// </summary>
  static SlotInformationStackLocation* CurrentStackLocation(void);

  /// <summary>
  /// Returns the stump in the current stack location, or null
  /// </summary>
  static SlotInformationStumpBase* CurrentStump(void);

  /// <summary>
  /// Registers the named slot with the current stack location
  /// </summary>
  static void RegisterSlot(DeferrableAutowiring* pAutowiring);

  /// <summary>
  /// Registers a NewAutoFilter with this SlotInformation
  /// </summary>
  static void RegisterSlot(const AutoFilterDescriptorStub& stub);

  // Operator overloads:
  void operator=(SlotInformationStackLocation&& rhs) = delete;
  void operator=(const SlotInformationStackLocation& rhs) = delete;
};
